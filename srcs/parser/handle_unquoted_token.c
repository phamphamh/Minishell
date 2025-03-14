/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/08 13:45:53 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Déplace l'index jusqu'à la fin du token.
 */
static int	move_to_token_end(const char *s, int i, t_split_env *env)
{
	if (env->is_heredoc)
		while (s[i] && s[i] != env->delimiter)
			i++;
	else
		while (s[i] && s[i] != env->delimiter && !is_quote(s[i]))
			i++;
	return (i);
}

/**
 * @brief Alloue et copie le token.
 */
static char	*allocate_token(const char *start, int len)
{
	char	*temp;

	temp = malloc(len + 1);
	if (!temp)
		return (NULL);
	strncpy(temp, start, len);
	temp[len] = '\0';
	return (temp);
}

/**
 * @brief Traite l'expansion du token selon l'environnement.
 */
static char	*process_token_expansion(char *temp, t_split_env *env)
{
	if (!env->is_heredoc)
		return (expand_env_vars(temp, env->ms, false, env->is_heredoc));
	return (ft_strdup(temp));
}

/**
 * @brief Stocke le token traité dans l'environnement.
 */
static void	store_expanded_token(char *expanded, t_split_env *env)
{
	env->tokens[env->token_count] = expanded;
	ft_gc_add(&env->ms->gc_head, expanded);
	env->token_count++;
}

/**
 * @brief Gère un token sans guillemets.
 */
int	handle_unquoted_token(const char *s, int i, t_split_env *env)
{
	const char	*start;
	int			token_len;
	char		*temp;
	char		*expanded;

	start = &s[i];
	i = move_to_token_end(s, i, env);
	token_len = &s[i] - start;
	temp = allocate_token(start, token_len);
	if (!temp)
		return (-1);
	expanded = process_token_expansion(temp, env);
	free(temp);
	if (!expanded)
		return (-1);
	store_expanded_token(expanded, env);
	return (i);
}
