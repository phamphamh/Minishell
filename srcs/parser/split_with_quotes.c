/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_with_quotes.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 11:36:12 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

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

static char	*add_space_after_heredoc(const char *s)
{
	char	*modified_input;
	int		i;
	int		j;

	modified_input = malloc(ft_strlen(s) * 2);
	if (!modified_input)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] == '<' && s[i + 1] == '<')
		{
			modified_input[j++] = s[i++];
			modified_input[j++] = s[i++];
			if (s[i] && s[i] != ' ')
				modified_input[j++] = ' ';
		}
		else
			modified_input[j++] = s[i++];
	}
	modified_input[j] = '\0';
	return (modified_input);
}

/**
 * @brief Découpe une chaîne en tokens en gérant les quotes.
 */
char	**ft_split_with_quotes(const char *s, char delimiter, t_minishell *ms,
		bool is_heredoc)
{
	t_split_env	env;
	char		*modified_input;

	env.token_count = 0;
	env.delimiter = delimiter;
	env.ms = ms;
	env.is_heredoc = is_heredoc;
	if (!s)
		return (NULL);
	env.tokens = malloc(sizeof(char *) * (ft_strlen(s) + 1));
	if (is_heredoc)
	{
		modified_input = add_space_after_heredoc(s);
		if (!modified_input)
			return (NULL);
	}
	else
		modified_input = ft_strdup(s);
	if (!env.tokens)
		return (NULL);
	ft_gc_add(&ms->gc_head, env.tokens);
	if (!process_tokens(modified_input, &env))
		return (NULL);
	env.tokens[env.token_count] = NULL;
	return (env.tokens);
}
