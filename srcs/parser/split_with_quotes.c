/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/04 21:04:08 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

bool	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

int	skip_quotes(const char *s, int i, char *quote)
{
	*quote = s[i++];
	while (s[i] && s[i] != *quote)
		i++;
	if (s[i] != *quote)
	{
		fprintf(stderr, "Syntax error: missing closing quote %c\n", *quote);
		return (-1);
	}
	return (i + 1);
}

/**
 * @brief Gère l'allocation et la libération des tokens.
 *
 * @param tokens Pointeur vers le tableau de tokens.
 * @param free_flag Indique s'il faut libérer la mémoire.
 * @param token_count Nombre de tokens alloués.
 * @param ms Structure principale du shell.
 * @return Pointeur vers `tokens` ou `NULL` en cas d'échec.
 */
static char	**manage_tokens(char **tokens, int free_flag, int *token_count, t_minishell *ms)
{
	if (free_flag && tokens)
	{
		// Supprime et libère chaque token
		while (--(*token_count) >= 0)
		{
			ft_gc_remove(&ms->gc_head, tokens[*token_count]);
			free(tokens[*token_count]);
		}
		// Supprime et libère le tableau des tokens
		ft_gc_remove(&ms->gc_head, tokens);
		free(tokens);
		return (NULL);
	}
	return (tokens);
}

static int	process_tokens(const char *s, char delimiter, char **tokens, int *token_count, t_minishell *ms)
{
	int	i = 0;
	int	prev_i;

	while (s[i])
	{
		prev_i = i;
		i = handle_token(s, i, delimiter, tokens, token_count, ms);
		if (i == -1)
			return ((int)(long)manage_tokens(tokens, 1, token_count, ms));
		if (i <= prev_i)
			break;
	}
	return (i);
}

char	**ft_split_with_quotes(const char *s, char delimiter, t_minishell *ms)
{
	char	**tokens;
	int		token_count = 0;

	if (!s)
		return (NULL);
	tokens = malloc(sizeof(char *) * (strlen(s) + 1));
	if (!tokens)
		return (NULL);
	ft_gc_add(&ms->gc_head, tokens);
	if (!process_tokens(s, delimiter, tokens, &token_count, ms))
		return (NULL);

	tokens[token_count] = NULL;
	return (tokens);
}
