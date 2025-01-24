/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/24 20:18:56 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/header.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

static int	skip_delimiters(const char *s, int i, char delimiter)
{
	while (s[i] == delimiter)
		i++;
	return (i);
}

static int	handle_quoted_token(const char *s, int i, char **tokens, int *token_count)
{
	char		quote;
	const char	*start;
	int			token_len;

	quote = s[i]; // Enregistre la quote
	start = &s[i + 1]; // Début du token après la quote
	i = skip_quotes(s, i, &quote); // Parcourt jusqu'à la fin de la quote
	if (i == -1) // Erreur si la quote fermante est manquante
		return (-1);
	token_len = &s[i - 1] - start; // Calcul de la longueur du token sans quotes
	tokens[*token_count] = allocate_token(start, token_len, tokens, *token_count);
	if (!tokens[*token_count])
		return (-1);
	(*token_count)++;
	return (i);
}

static int	handle_unquoted_token(const char *s, int i, char delimiter, char **tokens, int *token_count)
{
	const char	*start;
	int			token_len;

	start = &s[i];
	while (s[i] && s[i] != delimiter)
		i++;
	token_len = &s[i] - start;
	tokens[*token_count] = allocate_token(start, token_len, tokens, *token_count);
	if (!tokens[*token_count])
		return (-1);
	(*token_count)++;
	return (i);
}

static int	process_token(const char *s, int i, char delimiter, char **tokens, int *token_count)
{
	if (is_quote(s[i]))
		return (handle_quoted_token(s, i, tokens, token_count));
	else
		return (handle_unquoted_token(s, i, delimiter, tokens, token_count));
}

int	handle_token(const char *s, int i, char delimiter, char **tokens, int *token_count)
{
	i = skip_delimiters(s, i, delimiter);
	if (!s[i]) // Fin de la chaîne
		return (-1);
	return (process_token(s, i, delimiter, tokens, token_count));
}
