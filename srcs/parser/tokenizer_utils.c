/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 12:22:18 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**

	* @brief Remplit la chaîne étendue en
	* ajoutant des espaces autour des opérateurs.
 *
 * @param input Chaîne d'entrée
 * @param expanded Chaîne de sortie allouée
 */
static void	ft_update_quote_state(char c, char *in_quotes)
{
	if (is_quote(c))
	{
		if (*in_quotes == 0)
			*in_quotes = c;
		else if (*in_quotes == c)
			*in_quotes = 0;
	}
}

static void	ft_add_operator_with_spaces(t_expand_state *state)
{
	state->expanded[(*state->j)++] = ' ';
	state->expanded[(*state->j)++] = state->input[(*state->i)++];
	state->expanded[(*state->j)++] = state->input[(*state->i)++];
	state->expanded[(*state->j)++] = ' ';
}

static void	ft_add_char_with_spaces(t_expand_state *state)
{
	state->expanded[(*state->j)++] = ' ';
	state->expanded[(*state->j)++] = state->input[(*state->i)++];
	state->expanded[(*state->j)++] = ' ';
}

void	ft_process_character(t_expand_state *state, char *in_quotes)
{
	ft_update_quote_state(state->input[*(state->i)], in_quotes);
	if (!(*in_quotes) && ((state->input[*(state->i)] == '<'
				&& state->input[*(state->i) + 1] == '<')
			|| (state->input[*(state->i)] == '>' && state->input[*(state->i)
					+ 1] == '>')))
		ft_add_operator_with_spaces(state);
	else if (!(*in_quotes) && ft_is_operator(state->input[*(state->i)]))
		ft_add_char_with_spaces(state);
	else
		state->expanded[(*state->j)++] = state->input[(*state->i)++];
}

/**
 * @brief Détermine le type d'un token à partir de sa valeur.
 *
 * @param token Valeur du token
 * @param prev Token précédent (utile pour HEREDOC)
 * @return int Type du token (TOKEN_WORD, TOKEN_PIPE, etc.)
 */
int	ft_determine_token_type(char *token, int *is_cmd, t_token *prev)
{
	if (!token)
		return (TOKEN_EOF);
	if (ft_strcmp_trim(token, "|") == 0)
	{
		*is_cmd = 1;
		return (TOKEN_PIPE);
	}
	if (ft_strcmp_trim(token, "<<") == 0)
		return (TOKEN_HEREDOC);
	if (ft_strcmp_trim(token, "<") == 0)
		return (TOKEN_REDIR_IN);
	if (ft_strcmp_trim(token, ">") == 0)
		return (TOKEN_REDIR_OUT);
	if (ft_strcmp_trim(token, ">>") == 0)
		return (TOKEN_REDIR_APPEND);
	if (prev && prev->type == TOKEN_HEREDOC)
		return (TOKEN_EOF);
	if (*is_cmd && token[0] != '\0')
	{
		*is_cmd = 0;
		return (TOKEN_CMD);
	}
	return (TOKEN_WORD);
}
