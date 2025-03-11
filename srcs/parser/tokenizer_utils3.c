/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by tcousin          #+#    #+#              */
/*   Updated: 2025/03/10 19:12:57 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_handle_space_token(char *token)
{
	if (ft_strcmp_trim(token, "\1") == 0)
		return (1);
	return (0);
}

void	ft_concat_tokens(t_token *prev, char *token, t_minishell *minishell)
{
	char	*joined_value;

	joined_value = ft_strjoin(prev->value, token);
	if (!joined_value)
		return ;
	ft_gc_add(&minishell->gc_head, joined_value);
	prev->value = joined_value;
}

/*
 * brief: Ajoute un token à la fin de la liste de tokens
 *
 * token_list: Adresse de la liste de tokens
 * new_token: Nouveau token à ajouter
 * return: Liste de tokens mise à jour
 */
t_token	*ft_add_token(t_token **token_list, t_token *new_token)
{
	t_token	*current;

	if (!*token_list)
	{
		*token_list = new_token;
		return (*token_list);
	}
	current = *token_list;
	while (current->next)
		current = current->next;
	current->next = new_token;
	new_token->prev = current;
	return (*token_list);
}
