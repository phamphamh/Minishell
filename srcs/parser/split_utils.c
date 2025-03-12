/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/12 21:19:44 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Traite le contenu entre guillemets doubles,
 *        avec expansion des variables.
 */
static int	process_double_quotes(const char *s, int start, int end,
		t_split_env *env)
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
	expanded = expand_env_vars(content, env->ms, true, env->is_heredoc);
	free(content);
	if (!expanded)
		return (-1);
	env->tokens[env->token_count] = expanded;
	ft_gc_add(&env->ms->gc_head, expanded);
	env->token_count++;
	return (0);
}

/**
 * @brief Stocke un token entre guillemets.
 */
static int	store_quoted_token(const char *s, int start, int end,
		t_split_env *env)
{
	char	*temp;

	if (env->quote == '"')
		return (process_double_quotes(s, start, end, env));
	temp = malloc((end - start) + 1);
	if (!temp)
		return (-1);
	strncpy(temp, &s[start], end - start);
	temp[end - start] = '\0';
	env->tokens[env->token_count] = temp;
	ft_gc_add(&env->ms->gc_head, temp);
	env->token_count++;
	return (0);
}

/**
 * @brief Gère un token entre guillemets.
 */
int	handle_quoted_token(const char *s, int i, t_split_env *env)
{
	int	start;

	env->quote = s[i];
	start = i + 1;
	i = skip_quotes(s, i, &env->quote);
	if (i == -1)
		return (-1);
	if (start == i - 1)
	{
		return (i);
	}
	if (store_quoted_token(s, start, i - 1, env) == -1)
		return (-1);
	return (i);
}

/**
 * @brief Gère un token sans guillemets.
 */
static int	handle_unquoted_token(const char *s, int i, t_split_env *env)
{
	const char	*start;
	int			token_len;
	char		*temp;
	char		*expanded;

	start = &s[i];
	while (s[i] && s[i] != env->delimiter && !is_quote(s[i]))
		i++;
	token_len = &s[i] - start;
	temp = malloc(token_len + 1);
	if (!temp)
		return (-1);
	strncpy(temp, start, token_len);
	temp[token_len] = '\0';
	expanded = expand_env_vars(temp, env->ms, false, env->is_heredoc);
	free(temp);
	if (!expanded)
		return (-1);
	env->tokens[env->token_count] = expanded;
	ft_gc_add(&env->ms->gc_head, expanded);
	env->token_count++;
	return (i);
}

/**
 * @brief Gère l'extraction d'un token depuis une chaîne.
 */
int	handle_token(const char *s, int i, t_split_env *env)
{
	bool	has_space;

	has_space = false;
	while (s[i] == env->delimiter)
	{
		has_space = true;
		i++;
	}
	if (!s[i])
		return (i);
	if (has_space)
	{
		env->tokens[env->token_count] = ft_strdup("\1");
		ft_gc_add(&env->ms->gc_head, env->tokens[env->token_count]);
		env->token_count++;
	}
	if (is_quote(s[i]))
		return (handle_quoted_token(s, i, env));
	return (handle_unquoted_token(s, i, env));
}
