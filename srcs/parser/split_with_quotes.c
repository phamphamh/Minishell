/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 14:04:34 by tcousin          ###   ########.fr       */
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

static char	**manage_tokens(const char *s, int free_flag, char **tokens, int *token_count)
{
	if (free_flag)
	{
		while (--(*token_count) >= 0)
			free(tokens[*token_count]);
		free(tokens);
		return (NULL);
	}
	tokens = malloc(sizeof(char *) * (strlen(s) + 1));
	if (!tokens)
		return (NULL);
	return (tokens);
}

static int	process_tokens(const char *s, char delimiter, char **tokens, int *token_count, t_minishell *ms)
{
	int	i;
	int	prev_i;

	i = 0;
	while (s[i])
	{
		prev_i = i;
		i = handle_token(s, i, delimiter, tokens, token_count, ms);
		if (i == -1)
			return ((int)(long)manage_tokens(NULL, 1, tokens, token_count));
		if (i <= prev_i)
			break;
	}
	return (i);
}

char	**ft_split_with_quotes(const char *s, char delimiter, t_minishell *ms)
{
	char	**tokens;
	int		token_count;

	if (!s)
		return (NULL);
	tokens = manage_tokens(s, 0, NULL, &token_count);
	if (!tokens)
		return (NULL);
	token_count = 0;
	if (process_tokens(s, delimiter, tokens, &token_count, ms) == -1)
		return (NULL);
	tokens[token_count] = NULL;
	ft_gc_add(&ms->gc_head, tokens);
	return (tokens);
}
