/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 14:02:09 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/header.h"

static int	process_double_quotes(const char *s, int start, int end, char **tokens, int *token_count, t_minishell *ms)
{
	char	*content;
	char	*expanded;
	int		len;

	len = end - start;
	content = malloc(len + 1);
	if (!content)
		return (-1);
	strncpy(content, &s[start], len);
	content[len] = '\0';

	expanded = expand_env_vars(content, ms);
	free(content);
	if (!expanded)
		return (-1);
	tokens[*token_count] = expanded;
	ft_gc_add(&ms->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (0);
}

static int	store_quoted_token(const char *s, int start, int end, char quote, char **tokens, int *token_count, t_minishell *ms)
{
	if (quote == '"')
		return (process_double_quotes(s, start, end, tokens, token_count, ms));

	char *temp = malloc((end - start) + 1);
	if (!temp)
		return (-1);
	strncpy(temp, &s[start], end - start);
	temp[end - start] = '\0';

	tokens[*token_count] = temp;
	ft_gc_add(&ms->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (0);
}

int	handle_quoted_token(const char *s, int i, char **tokens, int *token_count, t_minishell *ms)
{
	char	quote;
	int		start;

	quote = s[i];
	start = i + 1;
	i = skip_quotes(s, i, &quote);
	if (i == -1)
		return (-1);
	if (store_quoted_token(s, start, i - 1, quote, tokens, token_count, ms) == -1)
		return (-1);
	return (i);
}

static int	handle_unquoted_token(const char *s, int i, char delimiter, char **tokens, int *token_count, t_minishell *ms)
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

	expanded = expand_env_vars(temp, ms);
	free(temp);
	if (!expanded)
		return (-1);
	tokens[*token_count] = expanded;
	ft_gc_add(&ms->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (i);
}

int	handle_token(const char *s, int i, char delimiter, char **tokens, int *token_count, t_minishell *ms)
{
	while (s[i] == delimiter)
		i++;
	if (!s[i])
		return (i);
	if (is_quote(s[i]))
		return (handle_quoted_token(s, i, tokens, token_count, ms));
	return (handle_unquoted_token(s, i, delimiter, tokens, token_count, ms));
}
