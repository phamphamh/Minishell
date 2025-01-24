/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/24 20:20:29 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

bool	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

char	*allocate_token(const char *start, int token_len, char **tokens, int token_count)
{
	char	*token;

	token = malloc(token_len + 1);
	if (!token)
	{
		while (--token_count >= 0)
			free(tokens[token_count]);
		free(tokens);
		return (NULL);
	}
	strncpy(token, start, token_len);
	token[token_len] = '\0';
	return (token);
}

int	skip_quotes(const char *s, int i, char *quote)
{
	*quote = s[i++]; // Stocke la quote d'ouverture
	while (s[i] && s[i] != *quote) // Parcourt jusqu'à la quote fermante ou la fin
		i++;
	if (s[i] != *quote) // Si la quote fermante est manquante
	{
		fprintf(stderr, "Syntax error: missing closing quote `%c`\n", *quote);
		return (-1); // Retourne une erreur
	}
	return (i + 1); // Passe après la quote fermante
}

char	**ft_split_with_quotes(const char *s, char delimiter)
{
	char	**tokens;
	int		token_count;
	int		i;

	if (!s)
		return (NULL);
	tokens = malloc(sizeof(char *) * (strlen(s) + 1));
	if (!tokens)
		return (NULL);
	token_count = 0;
	i = 0;
	while (s[i])
	{
		int prev_i = i;
		i = handle_token(s, i, delimiter, tokens, &token_count);
		if (i == -1)
			break ;
		// Ensure progress in the loop
		if (i <= prev_i)
			break ;
	}
	tokens[token_count] = NULL;
	return (tokens);
}

