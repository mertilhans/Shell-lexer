/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extension.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 01:44:24 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/03 01:58:54 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == '&' || 
            c == ';' || c == '(' || c == ')');
}

int is_quote(char c)
{
    return (c == '\'' || c == '"');
}
char *append_char(char *str, char c, int *len, int *capacity)
{
    char *new_str;

    if (*len >= *capacity - 1)
    {
        *capacity *= 2;
        new_str = malloc(*capacity);
        if (!new_str)
            return (NULL);
        if (str)
        {
            memcpy(new_str, str, *len);
            free(str);
        }
        str = new_str;
    }
    str[(*len)++] = c;
    str[*len] = '\0';
    return (str);
}
static void handle_quote(t_tokenizer *tokenizer, char *in_quote, char quote_char, char other_quote)
{
    if (tokenizer->current == quote_char && *in_quote != other_quote)
    {
        if (*in_quote == quote_char)
            *in_quote = '\0';
        else
            *in_quote = quote_char;
        lexer_advance(tokenizer);
    }
}

static char *process_character(t_tokenizer *tokenizer, char *word, int *len, int *capacity, char *in_quote)
{
    handle_quote(tokenizer, in_quote, '\'', '"');
    if (tokenizer->current == '\'')
        return (word);
    
    handle_quote(tokenizer, in_quote, '"', '\'');
    if (tokenizer->current == '"')
        return (word);
    
    word = append_char(word, tokenizer->current, len, capacity);
    if (!word)
        return (NULL);
    lexer_advance(tokenizer);
    return (word);
}

char *extract_complex_word(t_tokenizer *tokenizer)
{
    char *word;
    int len = 0;
    int capacity = 64;
    char in_quote = '\0';
    
    word = malloc(capacity);
    if (!word)
        return (NULL);
    word[0] = '\0';
    if (ft_ispace(tokenizer->current))
        return (word);
    
    while (tokenizer->current && 
           (in_quote || (!ft_ispace(tokenizer->current) && !is_special_char(tokenizer->current))))
    {
        word = process_character(tokenizer, word, &len, &capacity, &in_quote);
        if (!word)
            return (NULL);
    }
    return (word);
}
