/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 11:44:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/07 12:32:55 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/* je recois la valeur, le type, et la liste chainee du token
je complete la boucle en creant le token et en le mettant
dans la liste chainee
*/
bool	ft_integrate_token(char *value, int type, t_token **token_list, t_minishell *minishell)
{
	t_token	*new_token;
	t_token	*current;

	new_token = ft_create_token(value, type, minishell);
	if (!new_token)
		return (false);
	if (!*token_list)
		*token_list = new_token;
	else
	{
		current = *token_list;
		while (current->next)
			current = current->next;
		current->next = new_token;
	}
	return (true);
}

// gerer si les malloc echouent.
t_token	*ft_tokenize(char *input, t_minishell *minishell)
{
	char	**split_input;
	t_token	*token_list;
	int		i;
	int		type;

	token_list = NULL;
	i = 0;
	type = 0;
	split_input = ft_split(input, ' ');
	if (!split_input)
		return (NULL);
	while (split_input[i])
	{
		if (i == 0)
			type = ft_determine_token_type(split_input[i], NULL);
		else
			type = ft_determine_token_type(split_input[i], split_input[i - 1]);
		if (!ft_integrate_token(split_input[i], type, &token_list, minishell))
			return (ft_free_arrays(split_input), NULL);
		i++;
	}
	ft_free_arrays(split_input);
	return (token_list);
}

int	ft_determine_token_type(char *token, char *prev_token)
{
	if (!prev_token || ft_strcmp(prev_token, "|") == 0)
		return (CMD);
	if (ft_strcmp(token, "|") == 0)
		return (PIPE);
	if (ft_strcmp(token, "<") == 0)
		return (REDIR_IN);
	if (ft_strcmp(token, ">") == 0)
		return (REDIR_OUT);
	if (ft_strcmp(token, ">>") == 0)
		return (REDIR_APPEND);
	if (ft_strcmp(token, "<<") == 0)
		return (HERE_DOC);
	if ((prev_token && ft_strcmp(prev_token, "<") == 0)
		|| (prev_token && ft_strcmp(prev_token, ">") == 0)
		|| (prev_token && ft_strcmp(prev_token, ">>") == 0))
		return (FILENAME);
	if (prev_token && ft_strcmp(prev_token, "<<") == 0)
		return (LIM);
	return (ARG);
}

t_token	*ft_create_token(char *value, int type, t_minishell *minishell)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_gc_add(&minishell->gc_head, new_token);
	new_token->value = ft_strdup(value);
	/*
	if (!new_token->value)
	{
		ft_gc_remove(&minishell->gc_head, new_token);
		return (NULL);
	}
	*/
	ft_gc_add(&minishell->gc_head, new_token->value);
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

