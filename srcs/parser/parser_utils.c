/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousi          #+#    #+#           	  */
/*   Updated: 2025/02/15 12:49:47 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int ft_count_args(t_token *start)
{
    int count;
    t_token *current;

    count = 0;
    current = start;
    while (current && current->type != TOKEN_PIPE)
    {
        if (current->type != TOKEN_REDIR_IN && current->type != TOKEN_REDIR_OUT &&
            current->type != TOKEN_REDIR_APPEND)
        {
            count++;
        }
        current = current->next;
        if (current && (current->type == TOKEN_REDIR_IN || current->type == TOKEN_REDIR_OUT ||
                        current->type == TOKEN_REDIR_APPEND))
            current = current->next;
    }
    return (count);
}

t_redirection *ft_create_redirection(t_token *token, t_token *next, t_minishell *minishell)
{
    t_redirection *redir;

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
        free(redir);
        return (NULL);
    }
    ft_gc_add(&minishell->gc_head, redir->file);
    redir->next = NULL;
    return (redir);
}

void ft_process_redirection(t_token **current, t_cmd *cmd, t_minishell *minishell)
{
    t_redirection *redir;
	int		fd;

    redir = ft_create_redirection(*current, (*current)->next, minishell);
    if (!redir)
        return;
    if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
    {
		fd = -1;
        fd = open(redir->file, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0)
            close(fd);
        cmd->redirs = redir;
    }
    *current = (*current)->next ? (*current)->next->next : NULL;
}
