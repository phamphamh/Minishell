/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/07 12:48:30 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/**
 * @brief Vérifie si un caractère est une quote.
 */
bool	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

/**
 * @brief Passe les caractères entre quotes.
 */
int	skip_quotes(const char *s, int i, char *quote)
{
	*quote = s[i++];
	while (s[i] && s[i] != *quote)
		i++;
	if (s[i] != *quote)
	{
		ft_putstr_fd("Syntax error: missing closing quote ", 2);
		ft_putchar_fd(*quote, 2);
		ft_putstr_fd("\n", 2);
		return (-1);
	}
	return (i + 1);
}

/**
 * @brief Libère les tokens en cas d'échec.
 */
static char	**free_tokens(t_split_env *env, int free_flag)
{
	if (free_flag && env->tokens)
	{
		while (--(env->token_count) >= 0)
		{
			ft_gc_remove(&env->ms->gc_head, env->tokens[env->token_count]);
			free(env->tokens[env->token_count]);
		}
		ft_gc_remove(&env->ms->gc_head, env->tokens);
		free(env->tokens);
		return (NULL);
	}
	return (env->tokens);
}

/**
 * @brief Gère le découpage de la chaîne en tokens.
 */
static int	process_tokens(const char *s, t_split_env *env)
{
	int	i;
	int	prev_i;

	i = 0;
	while (s[i])
	{
		prev_i = i;
		i = handle_token(s, i, env);
		if (i == -1)
			return ((int)(long)free_tokens(env, 1));
		if (i <= prev_i)
			break ;
	}
	return (i);
}

/**
 * @brief Découpe une chaîne en tokens en gérant les quotes.
 */
char	**ft_split_with_quotes(const char *s, char delimiter, t_minishell *ms)
{
	t_split_env	env;

	env.token_count = 0;
	env.delimiter = delimiter;
	env.ms = ms;
	if (!s)
		return (NULL);
	env.tokens = malloc(sizeof(char *) * (ft_strlen(s) + 1));
	if (!env.tokens)
		return (NULL);
	ft_gc_add(&ms->gc_head, env.tokens);
	if (!process_tokens(s, &env))
		return (NULL);
	env.tokens[env.token_count] = NULL;
	return (env.tokens);
}
