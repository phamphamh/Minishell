/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/04 14:53:29 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Vérifie les erreurs de syntaxe des heredoc (<<).
 *
 * @param current Token en cours d'analyse.
 * @return int 1 si erreur de syntaxe, 0 sinon.
 */
static int	ft_check_heredoc_syntax(t_token *current)
{
	if (current->type == TOKEN_HEREDOC)
	{
		if (!current->next || current->next->type != TOKEN_EOF)
			return (ft_special_token_msg("newline"));
	}
	return (0);
}

/**
 * @brief Vérifie la validité syntaxique des redirections et pipes.
 *
 * @param current Token en cours d'analyse.
 * @return int 1 si erreur de syntaxe, 0 sinon.
 */
static int	ft_check_redir_syntax(t_token *current)
{
	if (current->type == TOKEN_PIPE && (!current->next
			|| current->next->type == TOKEN_PIPE))
		return (ft_special_token_msg("|"));
	if ((current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_IN
			|| current->type == TOKEN_REDIR_APPEND) && (!current->next
			|| current->next->type == TOKEN_PIPE))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		if (current->next)
			ft_putstr_fd(current->next->value, 2);
		else
			ft_putstr_fd("newline", 2);
		ft_putstr_fd("'\n", 2);
		return (1);
	}
	return (0);
}

/**
 * @brief Vérifie la validité syntaxique d'une séquence de tokens.
 *
 * @param current Premier token de la séquence.
 * @return int 1 si erreur de syntaxe, 0 sinon.
 */
static int	ft_check_token_sequence(t_token *current)
{
	while (current)
	{
		if (ft_check_special_token(current->value)
			|| ft_check_heredoc_syntax(current)
			|| ft_check_redir_syntax(current))
			return (1);
		current = current->next;
	}
	return (0);
}

/*
 * @brief Vérifie la validité syntaxique du premier token
 *
 * @param current Premier token à vérifier
 * @return int 1 si erreur de syntaxe, 0 sinon
 */
static int	ft_check_first_token(t_token *current)
{
	if (ft_check_special_token(current->value))
		return (1);
	if (current->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
	if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_IN
		|| current->type == TOKEN_REDIR_APPEND)
	{
		if (!current->next)
		{
			ft_putstr("minishll: ");
			ft_putstr_fd("syntax error near unexpected token `newline'\n", 2);
			return (1);
		}
		if (current->next->type == TOKEN_PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
				2);
			return (1);
		}
	}
	return (0);
}

/**
 * @brief Vérifie les erreurs de syntaxe dans une liste de tokens.
 *
 * @param tokens Liste de tokens à vérifier.
 * @return int 1 si erreur de syntaxe détectée, 0 sinon.
 */
int	ft_check_syntax_errors(t_token *tokens)
{
	if (!tokens)
		return (0);
	if (ft_check_first_token(tokens))
		return (1);
	return (ft_check_token_sequence(tokens));
}
