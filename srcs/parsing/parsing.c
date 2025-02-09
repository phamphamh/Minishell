/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 15:23:39 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

t_cmd	*tokens_to_cmds(t_token *tokens, t_minishell *minishell)
{
	t_cmd	*cmd_list;
	t_cmd	*current_cmd;

	cmd_list = NULL;
	current_cmd = NULL;
	while (tokens)
	{
		if (process_tokens(&cmd_list, &current_cmd, tokens, minishell))
			return (NULL);
		tokens = tokens->next;
	}
	if (current_cmd)
		append_cmd(&cmd_list, current_cmd);
	return (cmd_list);
}

int	ft_parse(char *input, t_minishell *minishell)
{
	(void)minishell;
	(void)input;
	t_token	*tokens;

	tokens = ft_tokenize(input, minishell);
	if (!tokens)
		return (1);
	if (tokens->type == PIPE)
	{
		ft_putstr_fd("Syntax error: unexpected token '|'\n", 2);
		return (1);
	}
	expand_variables(tokens, minishell);
	minishell->commands = tokens_to_cmds(tokens, minishell);
	return (!minishell->commands);
	//return(0);
}
