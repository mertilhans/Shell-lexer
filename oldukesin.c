#ifndef TOKENIZER_H
# define TOKENIZER_H

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
    TOKEN_AND,           // &&
    TOKEN_OR,            // ||
    TOKEN_SEMICOLON,     // ;
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
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
char        *extract_complex_word(t_tokenizer *tokenizer);

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

// String'e karakter ekleme (dinamik boyutlandırma ile)
static char *append_char(char *str, char c, int *len, int *capacity)
{
    if (*len >= *capacity - 1)
    {
        *capacity *= 2;
        str = realloc(str, *capacity);
        if (!str)
            return (NULL);
    }
    str[(*len)++] = c;
    str[*len] = '\0';
    return (str);
}

// Karmaşık kelime çıkarma (bash-like quote handling)
char *extract_complex_word(t_tokenizer *tokenizer)
{
    char *word;
    int len = 0;
    int capacity = 64;
    char in_quote = '\0';  // Hangi tırnak içinde olduğumuzu takip eder
    
    word = malloc(capacity);
    if (!word)
        return (NULL);
    word[0] = '\0';

    // Kelime başlangıcında boşluk varsa çık
    if (is_whitespace(tokenizer->current))
        return (word);

    while (tokenizer->current && 
           (in_quote || (!is_whitespace(tokenizer->current) && !is_special_char(tokenizer->current))))
    {
        if (tokenizer->current == '\'' && in_quote != '"')
        {
            // Tek tırnak işleme
            if (in_quote == '\'')
            {
                // Tek tırnak kapanıyor
                in_quote = '\0';
                advance(tokenizer);
            }
            else
            {
                // Tek tırnak açılıyor
                in_quote = '\'';
                advance(tokenizer);
            }
        }
        else if (tokenizer->current == '"' && in_quote != '\'')
        {
            // Çift tırnak işleme
            if (in_quote == '"')
            {
                // Çift tırnak kapanıyor
                in_quote = '\0';
                advance(tokenizer);
            }
            else
            {
                // Çift tırnak açılıyor
                in_quote = '"';
                advance(tokenizer);
            }
        }
        else
        {
            // Normal karakter - kelimeye ekle
            word = append_char(word, tokenizer->current, &len, &capacity);
            if (!word)
                return (NULL);
            advance(tokenizer);
        }
    }

    return (word);
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

    // Karmaşık kelimeler (tırnaklar dahil)
    char *word = extract_complex_word(tokenizer);
    if (word && strlen(word) > 0)
        return (create_token(TOKEN_WORD, word));
    
    if (word)
        free(word);

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
#include <readline/readline.h>
// Test fonksiyonu - çeşitli quote kombinasyonları test eder
int main()
{
	char *line;
	t_token *tokens;
	while(1)
	{
		line = readline("basardimanne->>>   ");

		tokens = tokenize_input(line);
		if(tokens)
		{
			print_tokens(tokens);
			token_list_free(tokens);
		}
		else
			printf("tokenization failed");
	}
}