/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_extension.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 16:01:25 by merilhan          #+#    #+#             */
/*   Updated: 2025/06/26 16:09:40 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

char    *ft_extract_word(t_tokenizer *tokenizer)
{
        int     start;
        int     len;
        char *word;

        start = tokenizer->pos;
        len = 0;
        while(tokenizer->current && !ft_ispace(tokenizer->current) &&
                !ft_is_special_char(tokenizer->current) && !ft_is_quote(tokenizer->current))
        {
                lexer_advance(tokenizer);
                len++;
        }
        if (len = 0)
                return (NULL);
        word = malloc(len + 1);
        if(!word)
                return (NULL);
        strncpy(word, tokenizer->input + start , len); // şuanlık strncpy güzel duruyor ama içeride kontrol yapmamız gerek o yüzden yazmadım
        word[len] = '\0';
        return (word);
}
int     ft_is_quote(char c)
{
        return(c == '\'' || c == '"');
}
