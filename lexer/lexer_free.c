/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 15:50:02 by merilhan          #+#    #+#             */
/*   Updated: 2025/06/26 15:50:59 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

void    tokenizer_free(t_tokenizer *tokenizer)
{
        if(!tokenizer)
                return(NULL);
        if (tokenizer->input)
                free(tokenizer->input);
        free(tokenizer);
}