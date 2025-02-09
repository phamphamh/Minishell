/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 15:40:59 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void print_errors(t_token *token)
{
    if (!token || !token->next)
    {
        ft_putstr_fd("syntax error near unexpected token `newline'\n", 2);
        return;
    }
    if (token->next->type == PIPE)
        ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
    else if (token->next->type == REDIR_OUT)
        ft_putstr_fd("syntax error near unexpected token `>'\n", 2);
    else if (token->next->type == REDIR_APPEND)
        ft_putstr_fd("syntax error near unexpected token `>>'\n", 2);
    else if (token->next->type == REDIR_IN)
        ft_putstr_fd("syntax error near unexpected token `<'\n", 2);
    else if (token->next->type == HERE_DOC)
        ft_putstr_fd("syntax error near unexpected token `<<'\n", 2);
    else
        ft_putstr_fd("syntax error near unexpected token `unknown'\n", 2);
}

static int	handle_existing_redirection(t_redirection **redir_list)
{
	if (*redir_list)
		*redir_list = NULL;
	return (0);
}

static int	allocate_redirection(t_redirection **redir, t_token *token, t_minishell *minishell)
{
	*redir = malloc(sizeof(t_redirection));
	if (!*redir)
		return (1);
	ft_gc_add(&minishell->gc_head, *redir);

	(*redir)->file = ft_strdup(token->next->value);
	if (!(*redir)->file)
		return (1);
	ft_gc_add(&minishell->gc_head, (*redir)->file);
	(*redir)->type = token->type;
	(*redir)->next = NULL;
	return (0);
}


int	add_redirection(t_redirection **redir_list, t_token *token, t_minishell *minishell)
{
	t_redirection	*redir;

	if (!token->next || !(token->next->type == FILENAME || token->next->type == LIM))
	{
		print_errors(token);
		return (1);
	}

	if (*redir_list && (token->type == REDIR_OUT || token->type == REDIR_APPEND))
		handle_existing_redirection(redir_list);

	if (allocate_redirection(&redir, token, minishell))
		return (1);

	// ✅ Ajout propre de la nouvelle redirection
	redir->next = *redir_list;
	*redir_list = redir;

	return (0);
}




