/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/07 13:23:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_count_args(t_token *start)
{
	int		count;
	t_token	*current;

	count = 0;
	current = start;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type != TOKEN_REDIR_IN && current->type != TOKEN_REDIR_OUT
			&& current->type != TOKEN_REDIR_APPEND)
		{
			count++;
			current = current->next;
		}
		else
		{
			current = current->next;
			if (current)
				current = current->next;
		}
	}
	return (count);
}

static t_redirection	*ft_create_redirection(t_token *token, t_token *next,
	t_minishell *minishell)
{
	t_redirection	*redir;

	if (!next)
		return (NULL);
	redir = malloc(sizeof(t_redirection));
	if (!redir)
		return (NULL);
	ft_gc_add(&minishell->gc_head, redir);
	redir->type = token->type;
	redir->file = ft_strdup(next->value);
	if (!redir->file)
	{
		ft_gc_remove(&minishell->gc_head, redir);
		return (NULL);
	}
	ft_gc_add(&minishell->gc_head, redir->file);
	redir->next = NULL;
	return (redir);
}

static t_cmd	*ft_create_command(t_token *start, t_minishell *minishell)
{
	t_cmd	*cmd;
	int		i;
	t_token	*current;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	ft_gc_add(&minishell->gc_head, cmd);
	cmd->args = malloc(sizeof(char *) * (ft_count_args(start) + 1));
	if (!cmd->args)
		return (NULL);
	ft_gc_add(&minishell->gc_head, cmd->args);
	i = 0;
	current = start;
	cmd->redirs = NULL;
	cmd->pipe_in = -1;
	cmd->pipe_out = -1;
	cmd->next = NULL;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type == TOKEN_REDIR_IN || current->type == TOKEN_REDIR_OUT
			|| current->type == TOKEN_REDIR_APPEND)
		{
			t_redirection *redir = ft_create_redirection(current,
					current->next, minishell);
			if (redir)
			{
				redir->next = cmd->redirs;
				cmd->redirs = redir;
			}
			current = current->next ? current->next->next : NULL;
		}
		else
		{
			cmd->args[i] = ft_strdup(current->value);
			if (!cmd->args[i])
				return (NULL);
			ft_gc_add(&minishell->gc_head, cmd->args[i]);
			i++;
			current = current->next;
		}
	}
	cmd->args[i] = NULL;
	cmd->name = cmd->args[0];
	return (cmd);
}

t_cmd	*tokens_to_cmds(t_token *tokens, t_minishell *minishell)
{
	t_cmd	*first_cmd;
	t_cmd	*current_cmd;
	t_token	*current_token;

	if (!tokens)
		return (NULL);
	first_cmd = ft_create_command(tokens, minishell);
	if (!first_cmd)
		return (NULL);
	current_cmd = first_cmd;
	current_token = tokens;
	while (current_token)
	{
		if (current_token->type == TOKEN_PIPE)
		{
			if (!current_token->next)
				break ;
			current_cmd->next = ft_create_command(current_token->next, minishell);
			if (!current_cmd->next)
				return (NULL);
			current_cmd = current_cmd->next;
		}
		current_token = current_token->next;
	}
	return (first_cmd);
} 
