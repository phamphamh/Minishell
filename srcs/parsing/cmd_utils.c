/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 15:20:08 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

t_cmd *init_cmd(t_minishell *minishell)
{
    t_cmd *cmd;
    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return NULL;
    ft_gc_add(&minishell->gc_head, cmd);
    cmd->args = NULL;
    cmd->input_redir = NULL;
    cmd->output_redir = NULL;
    cmd->is_pipeline = 0;
    cmd->next = NULL;
    return cmd;
}

void add_arg_to_cmd(t_cmd *cmd, char *arg, t_minishell *minishell)
{
    int i;
    int j;

    i = 0;
    while (cmd->args && cmd->args[i])
        i++;

    char **new_args = malloc(sizeof(char *) * (i + 2));
    if (!new_args)
        return;
    ft_gc_add(&minishell->gc_head, new_args);
    j = -1;
    while (++j < i)
        new_args[j] = cmd->args[j];
    new_args[i] = ft_strdup(arg);
    if (!new_args[i])
        return;
    ft_gc_add(&minishell->gc_head, new_args[i]);

    new_args[i + 1] = NULL;
    if (cmd->args)
        ft_gc_remove(&minishell->gc_head, cmd->args);
    free(cmd->args);

    cmd->args = new_args;
}



