/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/19 21:16:53 by tcousin          ###   ########.fr       */
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

    i = 0;
    while (cmd->args && cmd->args[i])
        i++;
    char **new_args = malloc(sizeof(char *) * (i + 2)); // +2 pour le nouvel arg et NULL
    if (!new_args)
        return;
    ft_gc_add(&minishell->gc_head, new_args);

    for (int j = 0; j < i; j++)
        new_args[j] = cmd->args[j];
    new_args[i] = ft_strdup(arg);
    new_args[i + 1] = NULL;

    ft_gc_add(&minishell->gc_head, new_args[i]);
    free(cmd->args);
    cmd->args = new_args;
}

void add_redirection(t_redirection **redir_list, t_token *token, t_minishell *minishell)
{
    t_redirection *redir;

    redir = malloc(sizeof(t_redirection));
    if (!redir)
        return;
    ft_gc_add(&minishell->gc_head, redir);

    redir->file = ft_strdup(token->next->value);
    ft_gc_add(&minishell->gc_head, redir->file);
    redir->type = token->type;
    redir->next = NULL;
    if (!*redir_list)
        *redir_list = redir;
    else
    {
        t_redirection *current = *redir_list;
        while (current->next)
            current = current->next;
        current->next = redir;
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

t_cmd *tokens_to_cmds(t_token *tokens, t_minishell *minishell)
{
    t_cmd *cmd_list;
    t_cmd *current_cmd;

    cmd_list = NULL;
    current_cmd = NULL;

    while (tokens)
    {
        if (!current_cmd) // Initialiser une nouvelle commande
            current_cmd = init_cmd(minishell);

        if (tokens->type == CMD || tokens->type == ARG)
            add_arg_to_cmd(current_cmd, tokens->value, minishell);
        else if (tokens->type == REDIR_IN || tokens->type == HERE_DOC)
            add_redirection(&current_cmd->input_redir, tokens, minishell);
        else if (tokens->type == REDIR_OUT || tokens->type == REDIR_APPEND)
            add_redirection(&current_cmd->output_redir, tokens, minishell);
        else if (tokens->type == PIPE)
        {
            current_cmd->is_pipeline = 1;
            append_cmd(&cmd_list, current_cmd);
            current_cmd = NULL; // Préparer pour la prochaine commande
        }
        tokens = tokens->next;
    }

    if (current_cmd) // Ajouter la dernière commande
        append_cmd(&cmd_list, current_cmd);

    return cmd_list;
}

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


