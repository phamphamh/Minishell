/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 13:59:16 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

bool	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

char	*allocate_token(t_minishell *minishell, const char *start, int token_len, char **tokens, int token_count)
{
	char	*token;

	token = malloc(token_len + 1);
	if (!token)
	{
		while (--token_count >= 0)
    		ft_gc_remove(&minishell->gc_head, tokens[token_count]); // Retire du GC proprement
		ft_gc_remove(&minishell->gc_head, tokens);

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

char	**ft_split_with_quotes(t_minishell *minishell, char *s, char delimiter)
{
	char	**tokens;
	int		token_count;
	int		i;

	if (!s)
		return (NULL);
	tokens = malloc(sizeof(char *) * (strlen(s) + 1));
	if (!tokens)
		return (NULL);
	ft_gc_add(&minishell->gc_head, tokens);  // ✅ Ajout au GC

	token_count = 0;
	i = 0;
	while (s[i])
	{
		int prev_i = i;
		i = handle_token(minishell, s, i, delimiter, tokens, &token_count);
		if (i == -1)
			break ;
		if (i <= prev_i)
			break ;
	}

	// ✅ Ajout de chaque token au GC
	for (int j = 0; j < token_count; j++)
		ft_gc_add(&minishell->gc_head, tokens[j]);

	tokens[token_count] = NULL;
	return (tokens);
}


