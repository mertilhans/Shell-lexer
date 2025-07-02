/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 01:19:53 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/03 01:46:07 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

t_tokenizer *tokenizer_init(char *input)
{
	t_tokenizer *tokenizer;

	if(!input)
		return (NULL);
	
	tokenizer = malloc(sizeof(t_tokenizer));
	if(!tokenizer)
		return (NULL);
	
	tokenizer->input = ft_strdup(input);
	if(!tokenizer->input)
	{
		free(tokenizer);
		return (NULL);
	}
	tokenizer->pos = 0;
	tokenizer->len ft_strlen(input);
	if(tokenizer->len > 0)
		tokenizer->current = input[0];
	else
		tokenizer->current = '\0'
	return (tokenizer);
}
t_token *create_token(t_token_type type, char *value)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    
    token->type = type;
    
    if (value)
        token->value = strdup(value);
    else
        token->value = NULL;
    
    token->next = NULL;
    
    return (token);
}
