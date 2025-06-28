#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND,
    TOKEN_HEREDOC,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_ERROR
} t_token_type;

typedef struct s_token
{
    t_token_type    type;
    char            *value;
    struct s_token  *next;
} t_token;

typedef struct s_tokenizer
{
    char    *input;
    int     pos;
    int     len;
    char    current;
} t_tokenizer;

// Function prototypes
t_tokenizer *tokenizer_init(char *input);
void        tokenizer_free(t_tokenizer *tokenizer);
t_token     *tokenizer_get_next_token(t_tokenizer *tokenizer);
t_token     *tokenize_input(char *input);
void        token_free(t_token *token);
void        token_list_free(t_token *tokens);
void        print_tokens(t_token *tokens);

// Helper functions
int         is_whitespace(char c);
int         is_special_char(char c);
int         is_quote(char c);
char        *extract_word(t_tokenizer *tokenizer);
char        *extract_quoted_string(t_tokenizer *tokenizer, char quote);

/* ========================= IMPLEMENTATION ========================= */

t_tokenizer *tokenizer_init(char *input)
{
    t_tokenizer *tokenizer;

    if (!input)
        return (NULL);
    tokenizer = malloc(sizeof(t_tokenizer));
    if (!tokenizer)
        return (NULL);
    tokenizer->input = strdup(input);
    if (!tokenizer->input)
    {
        free(tokenizer);
        return (NULL);
    }
    tokenizer->pos = 0;
    tokenizer->len = strlen(input);
    tokenizer->current = tokenizer->len > 0 ? input[0] : '\0';
    return (tokenizer);
}

void tokenizer_free(t_tokenizer *tokenizer)
{
    if (!tokenizer)
        return;
    if (tokenizer->input)
        free(tokenizer->input);
    free(tokenizer);
}

static void advance(t_tokenizer *tokenizer)
{
    if (tokenizer->pos < tokenizer->len - 1)
    {
        tokenizer->pos++;
        tokenizer->current = tokenizer->input[tokenizer->pos];
    }
    else
        tokenizer->current = '\0';
}

static char peek(t_tokenizer *tokenizer)
{
    if (tokenizer->pos + 1 < tokenizer->len)
        return (tokenizer->input[tokenizer->pos + 1]);
    return ('\0');
}

void skip_whitespace(t_tokenizer *tokenizer)
{
    while (tokenizer->current && is_whitespace(tokenizer->current))
        advance(tokenizer);
}

int is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == ';' || 
            c == '(' || c == ')');
}

int is_quote(char c)
{
    return (c == '\'' || c == '"');
}

static t_token *create_token(t_token_type type, char *value)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->next = NULL;
    return (token);
}

char *extract_word(t_tokenizer *tokenizer)
{
    int start = tokenizer->pos;
    int len = 0;
    char *word;

    while (tokenizer->current && !is_whitespace(tokenizer->current) && 
           !is_special_char(tokenizer->current) && !is_quote(tokenizer->current))
    {
        advance(tokenizer);
        len++;
    }
    if (len == 0)
        return (NULL);
    word = malloc(len + 1);
    if (!word)
        return (NULL);
    strncpy(word, tokenizer->input + start, len);
    word[len] = '\0';
    return (word);
}

char *extract_quoted_string(t_tokenizer *tokenizer, char quote)
{
    int start = tokenizer->pos + 1;
    int len = 0;
    char *str;

    advance(tokenizer);
    while (tokenizer->current && tokenizer->current != quote)
    {
        advance(tokenizer);
        len++;
    }
    if (tokenizer->current == quote)
        advance(tokenizer);
    str = malloc(len + 1);
    if (!str)
        return (NULL);
    strncpy(str, tokenizer->input + start, len);
    str[len] = '\0';
    return (str);
}

static t_token *handle_pipe(t_tokenizer *tokenizer)
{
    advance(tokenizer);
    return (create_token(TOKEN_PIPE, "|"));
}

static t_token *handle_redirect_in(t_tokenizer *tokenizer)
{
    if (peek(tokenizer) == '<')
    {
        advance(tokenizer);
        advance(tokenizer);
        return (create_token(TOKEN_HEREDOC, "<<"));
    }
    advance(tokenizer);
    return (create_token(TOKEN_REDIR_IN, "<"));
}

static t_token *handle_redirect_out(t_tokenizer *tokenizer)
{
    if (peek(tokenizer) == '>')
    {
        advance(tokenizer);
        advance(tokenizer);
        return (create_token(TOKEN_REDIR_APPEND, ">>"));
    }
    advance(tokenizer);
    return (create_token(TOKEN_REDIR_OUT, ">"));
}

static t_token *handle_special_chars(t_tokenizer *tokenizer)
{
    char c = tokenizer->current;
    
    if (c == '|')
        return (handle_pipe(tokenizer));
    if (c == '<')
        return (handle_redirect_in(tokenizer));
    if (c == '>')
        return (handle_redirect_out(tokenizer));
    if (c == ';')
    {
        advance(tokenizer);
        return (create_token(TOKEN_SEMICOLON, ";"));
    }
    if (c == '(')
    {
        advance(tokenizer);
        return (create_token(TOKEN_LPAREN, "("));
    }
    if (c == ')')
    {
        advance(tokenizer);
        return (create_token(TOKEN_RPAREN, ")"));
    }
    advance(tokenizer);
    return (create_token(TOKEN_ERROR, NULL));
}

t_token *tokenizer_get_next_token(t_tokenizer *tokenizer)
{
    char *word;

    if (!tokenizer)
        return (NULL);
    skip_whitespace(tokenizer);
    if (tokenizer->current == '\0')
        return (create_token(TOKEN_EOF, NULL));
    if (is_special_char(tokenizer->current))
        return (handle_special_chars(tokenizer));
    if (is_quote(tokenizer->current))
    {
        char quote = tokenizer->current;
        word = extract_quoted_string(tokenizer, quote);
        return (create_token(TOKEN_WORD, word));
    }
    word = extract_word(tokenizer);
    if (word)
        return (create_token(TOKEN_WORD, word));
    advance(tokenizer);
    return (create_token(TOKEN_ERROR, NULL));
}

t_token *tokenize_input(char *input)
{
    t_tokenizer *tokenizer;
    t_token *tokens = NULL;
    t_token *current = NULL;
    t_token *new_token;

    tokenizer = tokenizer_init(input);
    if (!tokenizer)
        return (NULL);
    while (1)
    {
        new_token = tokenizer_get_next_token(tokenizer);
        if (!new_token || new_token->type == TOKEN_EOF)
        {
            if (new_token)
                token_free(new_token);
            break;
        }
        if (!tokens)
            tokens = current = new_token;
        else
            current = current->next = new_token;
    }
    tokenizer_free(tokenizer);
    return (tokens);
}

void token_free(t_token *token)
{
    if (!token)
        return;
    if (token->value)
        free(token->value);
    free(token);
}

void token_list_free(t_token *tokens)
{
    t_token *current = tokens;
    t_token *next;

    while (current)
    {
        next = current->next;
        token_free(current);
        current = next;
    }
}

void print_tokens(t_token *tokens)
{
    t_token *current = tokens;
    const char *type_names[] = {
        "WORD", "PIPE", "REDIR_IN", "REDIR_OUT", "REDIR_APPEND",
        "HEREDOC", "SEMICOLON", "LPAREN", "RPAREN", "EOF", "ERROR"
    };

    printf("=== TOKENS ===\n");
    while (current)
    {
        printf("Type: %-12s Value: '%s'\n", 
               type_names[current->type], 
               current->value ? current->value : "NULL");
        current = current->next;
    }
    printf("==============\n");
}

int main(void)
{
    char input[] = "ls -la | grep \"test file\" > output.txt; echo 'done'";
    t_token *tokens;

    printf("Input: %s\n\n", input);
    tokens = tokenize_input(input);
    if (tokens)
    {
        print_tokens(tokens);
        token_list_free(tokens);
    }
    else
        printf("Tokenization failed!\n");
    return (0);
}