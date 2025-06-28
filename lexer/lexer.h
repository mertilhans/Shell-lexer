#ifndef LEXER_H
# define LEXER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef enum e_token_type
{
        TOKEN_WORD,
        TOKEN_PIPE,
        TOKEN_REDIR_IN,
        TOKEN_REDIR_OUT,
        TOKEN_REDIR_APPEND,
        TOKEN_HEREDOC,
        TOKEN_SEMICOLON,
        TOKEN_SEMICOLON,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_EOF,
        TOKEN_ERROR,
}       t_token_type;

typedef struct s_token
{
        t_token_type type;
        char    *value;
        struct s_token *next;
}       t_token;

typedef struct s_tokenizer
{
        char    *input;
        int     pos;
        int     len;
        char    current;
}       t_tokenizer;

t_tokenizer     *token_init(char *input);
void    ft_skipspace(t_tokenizer *tokenizer);
int    ft_ispace(char c);
int     ft_is_special_char(char c);
void     lexer_advance(t_tokenizer *tokenizer);

#endif