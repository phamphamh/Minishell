/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 13:41:33 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/header.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


static int	handle_quoted_token(const char *s, int i, char **tokens, int *token_count, t_minishell *minishell)
{
	char		quote;
	const char	*start;
	int			token_len;
	char		*expanded;
	char		*temp;

	quote = s[i];
	start = &s[i + 1];
	i = skip_quotes(s, i, &quote);
	if (i == -1)
		return (-1);
	token_len = &s[i - 1] - start;

	temp = malloc(token_len + 1);
	if (!temp)
		return (-1);
	strncpy(temp, start, token_len);
	temp[token_len] = '\0';

	if (quote == '"')
	{
		expanded = expand_env_vars(temp, minishell);
		free(temp);
		if (!expanded)
			return (-1);
		tokens[*token_count] = expanded;
	}
	else
	{
		tokens[*token_count] = temp;
	}

	if (!tokens[*token_count])
		return (-1);
	ft_gc_add(&minishell->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (i);
}

static int	handle_unquoted_token(const char *s, int i, char delimiter, char **tokens, int *token_count, t_minishell *minishell)
{
	const char	*start;
	int			token_len;
	char		*temp;
	char		*expanded;

	start = &s[i];
	while (s[i] && s[i] != delimiter && !is_quote(s[i]))
		i++;
	token_len = &s[i] - start;
	temp = malloc(token_len + 1);
	if (!temp)
		return (-1);
	strncpy(temp, start, token_len);
	temp[token_len] = '\0';

	expanded = expand_env_vars(temp, minishell);
	free(temp);
	if (!expanded)
		return (-1);
	tokens[*token_count] = expanded;
	if (!tokens[*token_count])
		return (-1);
	ft_gc_add(&minishell->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (i);
}

int	handle_token(const char *s, int i, char delimiter, char **tokens, int *token_count, t_minishell *minishell)
{
	while (s[i] == delimiter)
		i++;
	if (!s[i])
		return (i);

	if (is_quote(s[i]))
		return (handle_quoted_token(s, i, tokens, token_count, minishell));
	else
		return (handle_unquoted_token(s, i, delimiter, tokens, token_count, minishell));
}
