/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/07 18:06:46 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

char	*expand_single_token(char *token, t_minishell *minishell)
{
	char	*new_str;
	int		i;
	int		j;

	new_str = malloc(ft_strlen(token) * 2);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (token[i])
	{
		if (token[i] == '$' && (ft_isalpha(token[i + 1]) || token[i + 1] == '?' || token[i + 1] == '$'))
		{
			if (replace_variable(token, new_str, &i, &j, minishell) == -1)
				return (free(new_str), NULL);
		}
		else
			new_str[j++] = token[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}


void	expand_variables(t_token *tokens, t_minishell *minishell)
{
	char	*expanded_value;

	while (tokens)
	{
		if (tokens->type != HERE_DOC)
		{
			expanded_value = expand_single_token(tokens->value, minishell);
			free(tokens->value);
			tokens->value = expanded_value;
		}
		tokens = tokens->next;
	}
}


