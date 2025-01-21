/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/21 20:13:32 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

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
