//expendi parserden sonra yapın 
// cat << DOSYASONU(TIRNAKSIZ) SONRA DEĞİŞKEN OLARAK $HOME eof // tırnaksız eof yazarsan olur tırnakla yazarsan olmaz

// rl_catch signals ve rl_done ve rl_event_hook


//expendi parserden sonra yapın 
// cat << DOSYASONU(TIRNAKSIZ) SONRA DEĞİŞKEN OLARAK $HOME eof // tırnaksız eof yazarsan olur tırnakla yazarsan olmaz

// rl_catch signals ve rl_done ve rl_event_hook

#ifndef TOKENIZER_H
# define TOKENIZER_H
/*
Özellikler:

Modüler tasarım - her fonksiyon ayrı görevleri yerine getiriyor
Shell operatörlerini tanıyor: |, <, >, >>, <<, &&, ||, ;, ()
Tırnaklı stringleri doğru şekilde işliyor (' ve ")
Memory management dahil
Linked list yapısında token saklaması

Kullanım:

tokenize_input(input_string) ile tüm stringi tokenize edebilirsiniz
Veya tokenizer_init() ile başlatıp tokenizer_get_next_token() ile tek tek alabilirsiniz

Token Türleri:

TOKEN_WORD: Normal kelimeler ve komutlar
TOKEN_PIPE: |
TOKEN_REDIR_IN/OUT/APPEND: <, >, >>
TOKEN_HEREDOC: <<
TOKEN_AND/OR: &&, ||
TOKEN_SEMICOLON: ;
TOKEN_LPAREN/RPAREN: (, )

Bu yapı tamamen bağımsız çalışır ve global struct kullanmaz.*

*/
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

// Token türleri
typedef enum e_token_type
{
    TOKEN_WORD,          // Normal kelimeler
    TOKEN_PIPE,          // |
    TOKEN_REDIR_IN,      // <
    TOKEN_REDIR_OUT,     // >
    TOKEN_REDIR_APPEND,  // >>
    TOKEN_HEREDOC,       // <<
    TOKEN_EOF,           // Dosya sonu
    TOKEN_ERROR          // Hata
} t_token_type;

// Token yapısı
typedef struct s_token
{
    t_token_type    type;
    char            *value;
    struct s_token  *next;
} t_token;

// Tokenizer yapısı
typedef struct s_tokenizer
{
    char    *input;      // Giriş metni
    int     pos;         // Mevcut pozisyon
    int     len;         // Toplam uzunluk
    char    current;     // Mevcut karakter
} t_tokenizer;

// Fonksiyon prototipleri
t_tokenizer *tokenizer_init(char *input);
void        tokenizer_free(t_tokenizer *tokenizer);
t_token     *tokenizer_get_next_token(t_tokenizer *tokenizer);
t_token     *tokenize_input(char *input);
void        token_free(t_token *token);
void        token_list_free(t_token *tokens);
void        print_tokens(t_token *tokens);

// Yardımcı fonksiyonlar
int         is_whitespace(char c);
int         is_special_char(char c);
int         is_quote(char c);
char        *extract_word(t_tokenizer *tokenizer);
char        *extract_quoted_string(t_tokenizer *tokenizer, char quote);

#endif

/* ========================= IMPLEMENTATION ========================= */

// Tokenizer başlatma
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

// Tokenizer bellek temizleme
void tokenizer_free(t_tokenizer *tokenizer)
{
    if (!tokenizer)
        return;
    
    if (tokenizer->input)
        free(tokenizer->input);
    free(tokenizer);
}

// Bir sonraki karaktere geç
static void advance(t_tokenizer *tokenizer)
{
    if (tokenizer->pos < tokenizer->len - 1)
    {
        tokenizer->pos++;
        tokenizer->current = tokenizer->input[tokenizer->pos];
    }
    else
    {
        tokenizer->current = '\0';
    }
}

// Bir sonraki karaktere bak (advance etmeden)
static char peek(t_tokenizer *tokenizer)
{
    if (tokenizer->pos + 1 < tokenizer->len)
        return (tokenizer->input[tokenizer->pos + 1]);
    return ('\0');
}

// Boşluk karakterlerini atla
void skip_whitespace(t_tokenizer *tokenizer)
{
    while (tokenizer->current && is_whitespace(tokenizer->current))
        advance(tokenizer);
}

// Karakter kontrolü fonksiyonları
int is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == '&' || 
            c == ';' || c == '(' || c == ')');
}

int is_quote(char c)
{
    return (c == '\'' || c == '"');
}

// Token oluşturma
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

// Kelime çıkarma (quotes olmadan)
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

// Tırnaklı string çıkarma
char *extract_quoted_string(t_tokenizer *tokenizer, char quote)
{
    int start = tokenizer->pos + 1; // Başlangıç tırnağını atla
    int len = 0;
    char *str;

    advance(tokenizer); // Başlangıç tırnağını geç

    while (tokenizer->current && tokenizer->current != quote)
    {
        advance(tokenizer);
        len++;
    }

    if (tokenizer->current == quote)
        advance(tokenizer); // Bitiş tırnağını geç

    str = malloc(len + 1);
    if (!str)
        return (NULL);

    strncpy(str, tokenizer->input + start, len);
    str[len] = '\0';

    return (str);
}

// Bir sonraki token'ı al
t_token *tokenizer_get_next_token(t_tokenizer *tokenizer)
{
    if (!tokenizer)
        return (NULL);

    skip_whitespace(tokenizer);

    if (tokenizer->current == '\0')
        return (create_token(TOKEN_EOF, NULL));

    // Pipe |
    if (tokenizer->current == '|')
    {
        if (peek(tokenizer) == '|')
        {
            advance(tokenizer);
            advance(tokenizer);
            return (create_token(TOKEN_OR, "||"));
        }
        advance(tokenizer);
        return (create_token(TOKEN_PIPE, "|"));
    }

    // Redirect <, <<
    if (tokenizer->current == '<')
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

    // Redirect >, >>
    if (tokenizer->current == '>')
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

    // And &&
    if (tokenizer->current == '&')
    {
        if (peek(tokenizer) == '&')
        {
            advance(tokenizer);
            advance(tokenizer);
            return (create_token(TOKEN_AND, "&&"));
        }
        advance(tokenizer);
        return (create_token(TOKEN_ERROR, "&"));
    }

    // Semicolon ;
    if (tokenizer->current == ';')
    {
        advance(tokenizer);
        return (create_token(TOKEN_SEMICOLON, ";"));
    }

    // Parentheses
    if (tokenizer->current == '(')
    {
        advance(tokenizer);
        return (create_token(TOKEN_LPAREN, "("));
    }

    if (tokenizer->current == ')')
    {
        advance(tokenizer);
        return (create_token(TOKEN_RPAREN, ")"));
    }

    // Quoted strings
    if (is_quote(tokenizer->current))
    {
        char quote = tokenizer->current;
        char *str = extract_quoted_string(tokenizer, quote);
        return (create_token(TOKEN_WORD, str));
    }

    // Normal words
    char *word = extract_word(tokenizer);
    if (word)
        return (create_token(TOKEN_WORD, word));

    // Bilinmeyen karakter
    advance(tokenizer);
    return (create_token(TOKEN_ERROR, NULL));
}

// Tüm input'u tokenize et
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
        if (!new_token)
            break;

        if (new_token->type == TOKEN_EOF)
        {
            token_free(new_token);
            break;
        }

        if (!tokens)
        {
            tokens = new_token;
            current = tokens;
        }
        else
        {
            current->next = new_token;
            current = new_token;
        }
    }

    tokenizer_free(tokenizer);
    return (tokens);
}

// Token bellek temizleme
void token_free(t_token *token)
{
    if (!token)
        return;
    
    if (token->value)
        free(token->value);
    free(token);
}

// Token listesi bellek temizleme
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

// Token'ları yazdır (debug için)
void print_tokens(t_token *tokens)
{
    t_token *current = tokens;
    const char *type_names[] = {
        "WORD", "PIPE", "REDIR_IN", "REDIR_OUT", "REDIR_APPEND",
        "HEREDOC", "AND", "OR", "SEMICOLON", "LPAREN", "RPAREN",
        "EOF", "ERROR"
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

// Test fonksiyonu
int main(void)
{
    char input[] = "ls -la | grep \"test file\" > output.txt && echo 'done'";
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