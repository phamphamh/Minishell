/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/31 13:43:30 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void print_commands(t_cmd *cmd_list)
{
    t_cmd *cmd;

    cmd = cmd_list;

    while (cmd)
    {
        printf("Command:\n");
        if (cmd->args)
        {
            printf("  Arguments:\n");
            for (int i = 0; cmd->args[i]; i++)
                printf("    %s\n", cmd->args[i]);
        }
        if (cmd->input_redir)
        {
            printf("  Input redirections:\n");
            t_redirection *in = cmd->input_redir;
            while (in)
            {
                printf("    Type: %d, File: %s\n", in->type, in->file);
                in = in->next;
            }
        }
        if (cmd->output_redir)
        {
            printf("  Output redirections:\n");
            t_redirection *out = cmd->output_redir;
            while (out)
            {
                printf("    Type: %d, File: %s\n", out->type, out->file);
                out = out->next;
            }
        }
        printf("  Is pipeline: %d\n", cmd->is_pipeline);
        cmd = cmd->next;
    }
}

void	append_cmd(t_cmd **cmd_list, t_cmd *cmd)
{
	t_cmd	*current;

	if (!*cmd_list)
	{
		*cmd_list = cmd;
		return ;
	}
	current = *cmd_list;
	while (current->next)
		current = current->next;
	current->next = cmd;
}

int	handle_redirections(t_cmd *cmd, t_token *token, t_minishell *minishell)
{
	if (token->type == REDIR_IN || token->type == HERE_DOC)
	{
		if (add_redirection(&cmd->input_redir, token, minishell))
			return (1);
	}
	else if (token->type == REDIR_OUT || token->type == REDIR_APPEND)
	{
		if (add_redirection(&cmd->output_redir, token, minishell))
			return (1);
	}
	return (0);
}

int	process_command_token(t_cmd *cmd, t_token *token, t_minishell *minishell)
{
	if (token->type == CMD || token->type == ARG)
	{
		if (!token->prev || !(token->prev->type == REDIR_OUT ||
				token->prev->type == REDIR_APPEND || token->prev->type == REDIR_IN))
			add_arg_to_cmd(cmd, token->value, minishell);
	}
	else
		return (handle_redirections(cmd, token, minishell));
	return (0);
}

void	handle_pipe(t_cmd **cmd_list, t_cmd **current_cmd)
{
	(*current_cmd)->is_pipeline = 1;
	append_cmd(cmd_list, *current_cmd);
	*current_cmd = NULL;
}

int	process_tokens(t_cmd **cmd_list, t_cmd **current_cmd,
							t_token *tokens, t_minishell *minishell)
{
	if (tokens->type == PIPE)
	{
		if (!tokens->next || tokens->next->type == PIPE)
		{
			ft_putstr_fd("Syntax error: unexpected token '|'\n", 2);
			return (1);
		}
		handle_pipe(cmd_list, current_cmd);
	}
	else
	{
		if (!*current_cmd)
			*current_cmd = init_cmd(minishell);
		if (process_command_token(*current_cmd, tokens, minishell))
			return (1);
	}
	return (0);
}
