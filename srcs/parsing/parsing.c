/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/21 20:17:27 by tcousin          ###   ########.fr       */
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

void append_cmd(t_cmd **cmd_list, t_cmd *cmd)
{
    if (!*cmd_list)
        *cmd_list = cmd;
    else
    {
        t_cmd *current = *cmd_list;
        while (current->next)
            current = current->next;
        current->next = cmd;
    }
}

static void	process_token(t_cmd **current_cmd, t_token *token, t_minishell *minishell)
{
	if (token->type == CMD || token->type == ARG)
		add_arg_to_cmd(*current_cmd, token->value, minishell);
	else if (token->type == REDIR_IN || token->type == HERE_DOC)
		add_redirection(&(*current_cmd)->input_redir, token, minishell);
	else if (token->type == REDIR_OUT || token->type == REDIR_APPEND)
		add_redirection(&(*current_cmd)->output_redir, token, minishell);
}

static void	handle_pipe(t_cmd **cmd_list, t_cmd **current_cmd)
{
	(*current_cmd)->is_pipeline = 1;
	append_cmd(cmd_list, *current_cmd);
	*current_cmd = NULL;
}

t_cmd	*tokens_to_cmds(t_token *tokens, t_minishell *minishell)
{
	t_cmd	*cmd_list;
	t_cmd	*current_cmd;

	cmd_list = NULL;
	current_cmd = NULL;
	while (tokens)
	{
		if (!current_cmd)
			current_cmd = init_cmd(minishell);
		if (tokens->type == PIPE)
			handle_pipe(&cmd_list, &current_cmd);
		else
			process_token(&current_cmd, tokens, minishell);
		tokens = tokens->next;
	}
	if (current_cmd)
		append_cmd(&cmd_list, current_cmd);
	return (cmd_list);
}

void    ft_parse(char *input, t_minishell *minishell)
{
    t_token *tokens;

    tokens = ft_tokenize(input, minishell);
    minishell->commands = tokens_to_cmds(tokens, minishell);
    if (!minishell->commands)
    {
        ft_putstr_fd("Error: Failed to create command structures\n", 2);
        return;
    }    printf("\n--- Parsed Commands ---\n");
}


