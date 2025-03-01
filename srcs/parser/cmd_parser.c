/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/01 16:19:36 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"


static t_cmd *ft_init_command(t_minishell *minishell, int argc)
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return (NULL);
    ft_gc_add(&minishell->gc_head, cmd);
    cmd->args = malloc(sizeof(char *) * (argc + 1));
    if (!cmd->args)
    {
        ft_gc_remove(&minishell->gc_head, cmd);
        free(cmd);
        return (NULL);
    }
    ft_gc_add(&minishell->gc_head, cmd->args);
    cmd->redirs = NULL;
    cmd->pipe_in = -1;
    cmd->pipe_out = -1;
    cmd->next = NULL;
    return (cmd);
}

static void ft_process_argument(t_token **current, t_cmd *cmd, int *i, t_minishell *minishell)
{
    cmd->args[*i] = ft_strdup((*current)->value);
    if (!cmd->args[*i])
        return;
    ft_gc_add(&minishell->gc_head, cmd->args[*i]);
    (*i)++;
    *current = (*current)->next;
}

static t_cmd *ft_create_command(t_token *start, t_minishell *minishell)
{
    t_cmd *cmd;
    int i;
    t_token *current;

    cmd = ft_init_command(minishell, ft_count_args(start));
    if (!cmd)
        return (NULL);
    current = start;
    i = 0;
    while (current && current->type != TOKEN_PIPE)
    {
        if (current->type == TOKEN_REDIR_IN || current->type == TOKEN_REDIR_OUT ||
            current->type == TOKEN_REDIR_APPEND || current->type == TOKEN_HEREDOC)
            ft_process_redirection(&current, cmd, minishell);
        else
            ft_process_argument(&current, cmd, &i, minishell);
    }
    cmd->args[i] = NULL;
    cmd->name = cmd->args[0];
    return (cmd);
}


t_cmd *tokens_to_cmds(t_token *tokens, t_minishell *minishell)
{
    t_cmd *first_cmd;
    t_cmd *current_cmd;
    t_token *current_token;

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
                break;
            current_cmd->next = ft_create_command(current_token->next, minishell);
            if (!current_cmd->next)
            {
                ft_gc_remove_cmds(&minishell->gc_head, first_cmd);
                return (NULL);
            }
            current_cmd = current_cmd->next;
        }
        current_token = current_token->next;
    }
    return (first_cmd);
}

