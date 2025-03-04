/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/04 21:03:48 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Traite le contenu entre guillemets doubles, avec expansion des variables
 *
 * @param s Chaîne de caractères source
 * @param start Indice de début du contenu
 * @param end Indice de fin du contenu
 * @param tokens Tableau de tokens à remplir
 * @param token_count Pointeur vers le compteur de tokens
 * @param ms Structure principale du shell
 * @return int 0 si succès, -1 en cas d'erreur
 */
static int	process_double_quotes(const char *s, int start, int end,
				char **tokens, int *token_count, t_minishell *ms)
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

/**
 * @brief Stocke un token entre guillemets
 *
 * @param s Chaîne de caractères source
 * @param start Indice de début du contenu
 * @param end Indice de fin du contenu
 * @param quote Type de guillemet (simple ou double)
 * @param tokens Tableau de tokens à remplir
 * @param token_count Pointeur vers le compteur de tokens
 * @param ms Structure principale du shell
 * @return int 0 si succès, -1 en cas d'erreur
 */
static int	store_quoted_token(const char *s, int start, int end, char quote,
				char **tokens, int *token_count, t_minishell *ms)
{
	char	*temp;

	if (quote == '"')
		return (process_double_quotes(s, start, end, tokens, token_count, ms));
	temp = malloc((end - start) + 1);
	if (!temp)
		return (-1);
	strncpy(temp, &s[start], end - start);
	temp[end - start] = '\0';
	tokens[*token_count] = temp;
	ft_gc_add(&ms->gc_head, tokens[*token_count]);
	(*token_count)++;
	return (0);
}

/**
 * @brief Gère un token entre guillemets
 *
 * @param s Chaîne de caractères source
 * @param i Indice courant dans la chaîne
 * @param tokens Tableau de tokens à remplir
 * @param token_count Pointeur vers le compteur de tokens
 * @param ms Structure principale du shell
 * @return int Nouvel indice après traitement, -1 en cas d'erreur
 */
int	handle_quoted_token(const char *s, int i, char **tokens,
		int *token_count, t_minishell *ms)
{
	char	quote;
	int		start;

	quote = s[i];
	start = i + 1;
	i = skip_quotes(s, i, &quote);
	if (i == -1)
		return (-1);
	if (start == i - 1)
	{
		tokens[*token_count] = ft_strdup(quote == '"' ? "\"\"" : "''");
		if (!tokens[*token_count])
			return (-1);
		ft_gc_add(&ms->gc_head, tokens[*token_count]);
		(*token_count)++;
		return (i);
	}
	if (store_quoted_token(s, start, i - 1, quote, tokens, token_count, ms) == -1)
		return (-1);
	return (i);
}

/**
 * @brief Gère un token sans guillemets
 *
 * @param s Chaîne de caractères source
 * @param i Indice courant dans la chaîne
 * @param delimiter Délimiteur de token
 * @param tokens Tableau de tokens à remplir
 * @param token_count Pointeur vers le compteur de tokens
 * @param ms Structure principale du shell
 * @return int Nouvel indice après traitement, -1 en cas d'erreur
 */
static int	handle_unquoted_token(const char *s, int i, char delimiter,
				char **tokens, int *token_count, t_minishell *ms)
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

/**
 * @brief Gère l'extraction d'un token depuis une chaîne
 *
 * @param s Chaîne de caractères source
 * @param i Indice courant dans la chaîne
 * @param delimiter Délimiteur de token
 * @param tokens Tableau de tokens à remplir
 * @param token_count Pointeur vers le compteur de tokens
 * @param ms Structure principale du shell
 * @return int Nouvel indice après traitement, -1 en cas d'erreur
 */
int	handle_token(const char *s, int i, char delimiter, char **tokens,
		int *token_count, t_minishell *ms)
{
	while (s[i] == delimiter)
		i++;
	if (!s[i])
		return (i);
	if (is_quote(s[i]))
		return (handle_quoted_token(s, i, tokens, token_count, ms));
	return (handle_unquoted_token(s, i, delimiter, tokens, token_count, ms));
}
