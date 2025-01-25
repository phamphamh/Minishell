/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 11:44:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/25 14:05:47 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

static bool is_redirection_type(t_token *prev_token)
{
	if (!prev_token)
		return (false);
	if (prev_token->type == REDIR_IN
		|| prev_token->type == REDIR_OUT
		|| prev_token->type == REDIR_APPEND)
		return (true);
	return (false);
}

static bool	is_here_doc_type(t_token *prev_token)
{
	if (!prev_token)
		return (false);
	if (prev_token->type == HERE_DOC)
		return (true);
	return (false);
}

static int	check_token_types(char *token)
{
	if (ft_strcmp(token, "|") == 0)
		return (PIPE);
	if (ft_strlen(token) == 2 && ft_strcmp(token, ">>") == 0)
		return (REDIR_APPEND);
	if (ft_strlen(token) == 1 && ft_strcmp(token, ">") == 0)
		return (REDIR_OUT);
	if (ft_strcmp(token, "<") == 0)
		return (REDIR_IN);
	if (ft_strcmp(token, "<<") == 0)
		return (HERE_DOC);
	return (-1);
}

static int	check_prev_token(t_token *prev_token)
{
	if (is_redirection_type(prev_token))
		return (FILENAME);
	if (is_here_doc_type(prev_token))
		return (LIM);
	if (prev_token && prev_token->type == FILENAME)
		return (FILENAME);
	return (-1);
}

int	ft_determine_token_type(char *token, t_token *prev_token)
{
	int	token_type;

	if (!prev_token)
	{
		if (is_command(token, NULL))
			return (CMD);
	}
	else if (is_command(token, prev_token->value))
		return (CMD);

	token_type = check_token_types(token);
	if (token_type != -1)
		return (token_type);

	token_type = check_prev_token(prev_token);
	if (token_type != -1)
		return (token_type);

	return (ARG);
}
