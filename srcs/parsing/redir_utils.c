/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/24 11:06:14 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

int add_redirection(t_redirection **redir_list, t_token *token, t_minishell *minishell)
{
    t_redirection *redir;

    // Vérification que le token suivant existe
    if (!token->next || token->next->type != FILENAME)
    {
        ft_putstr_fd("syntax error near unexpected token `newline'\n", 2);
        return (1);
    }

    // Allocation de la redirection
    redir = malloc(sizeof(t_redirection));
    if (!redir)
        return (1);
    ft_gc_add(&minishell->gc_head, redir);

    // Duplication du nom de fichier
    redir->file = ft_strdup(token->next->value);
    if (!redir->file)
    {
        ft_putstr_fd("Error: Memory allocation failed\n", 2);
        return (1);
    }
    ft_gc_add(&minishell->gc_head, redir->file);

    // Initialisation des champs de la redirection
    redir->type = token->type;
    redir->next = NULL;

    // Ajout à la liste des redirections
    if (!*redir_list)
        *redir_list = redir;
    else
    {
        t_redirection *current = *redir_list;
        while (current->next)
            current = current->next;
        current->next = redir;
    }
    return (0);
}

