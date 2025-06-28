/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 15:51:23 by merilhan          #+#    #+#             */
/*   Updated: 2025/06/26 16:07:52 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

void     lexer_advance(t_tokenizer *tokenizer)
{
        if(tokenizer->pos < tokenizer->len -1)
        {
                tokenizer->pos++;
                tokenizer->current = tokenizer->input[tokenizer->pos];
        }
        else
                tokenizer->current = '\0';
}
static char lexer_peek(t_tokenizer *tokenizer)
{
        if (tokenizer->pos +1 < tokenizer->len)
                return (tokenizer->input[tokenizer->pos + 1]);
        return('\0');
}
int    ft_ispace(char c)
{
        return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}
void    ft_skipspace(t_tokenizer *tokenizer)
{
        while(tokenizer->current && ft_ispace(tokenizer->current))
                lexer_advance(tokenizer);
}
int     ft_is_special_char(char c)
{
        return(c == '|' || c == '<' || c == '>');
}