/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 15:43:47 by merilhan          #+#    #+#             */
/*   Updated: 2025/06/26 16:04:39 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

static t_token *create_token(t_token_type type, char *value)
{
        t_token *token;
        token = malloc(sizeof(t_token));
        if(!token)
                return (NULL);
        token->type = type;
        if (value)
                token->value = strdup(value); // ft_strdup yazcaz unutmıyım zor geldi şimdi 
        else
                token->value = NULL;
        token->next = NULL;
        return (token);
}

t_tokenizer     *token_init(char *input)
{
        t_tokenizer *tokenizer;
        
        if(!input)
                return (NULL);
        tokenizer = malloc(sizeof(t_token));
        if(!tokenizer)
                return(NULL);
        tokenizer->input = strdup(input); // strdup yazıcaz unutmayayım diye not aldım
        if(!tokenizer->input)
        {
                free(tokenizer);
                return (NULL);
        }
        tokenizer->pos = 0;
        tokenizer->len = strlen(input); // ft_strlen yazıcam unutmayayım 
        if(tokenizer->len > 0)
                tokenizer->current = input[0];
        else
                tokenizer->current = '\0';
        return (tokenizer);
}
