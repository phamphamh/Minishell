/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 12:43:39 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int ft_check_first_token(t_token *current)
{
    if (current->type == TOKEN_PIPE)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (1);
    }
    if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_IN ||
        current->type == TOKEN_REDIR_APPEND)
    {
        if (!current->next)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
            return (1);
        }
        if (current->next->type == TOKEN_PIPE)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
            return (1);
        }
    }
    return (0);
}

static int ft_check_token_sequence(t_token *current)
{
    while (current)
    {
        if (current->type == TOKEN_PIPE && (!current->next || current->next->type == TOKEN_PIPE))
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
            return (1);
        }
        if ((current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_IN ||
            current->type == TOKEN_REDIR_APPEND) && (!current->next || current->next->type == TOKEN_PIPE))
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
            if (current->next)
                ft_putstr_fd(current->next->value, 2);
            else
                ft_putstr_fd("newline", 2);
            ft_putstr_fd("'\n", 2);
            return (1);
        }
        current = current->next;
    }
    return (0);
}

int ft_check_syntax_errors(t_token *tokens)
{
    if (!tokens)
        return (0);
    if (ft_check_first_token(tokens))
        return (1);
    return (ft_check_token_sequence(tokens));
}


