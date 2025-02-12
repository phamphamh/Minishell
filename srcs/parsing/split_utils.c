/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/10 16:12:56 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/header.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char *expand_env_vars(const char *str, t_minishell *minishell)
{
	char *result;
	char *var_name;
	char *var_value;
	int i, j, k;
	int len;
	bool in_single_quotes;

	// Calculer la taille maximale nécessaire
	len = strlen(str) * 2;  // Pour gérer le pire cas
	result = malloc(len + 1);
	if (!result)
		return NULL;

	i = 0;  // Index dans str
	j = 0;  // Index dans result
	in_single_quotes = false;
	while (str[i])
	{
		if (str[i] == '\'')
			in_single_quotes = !in_single_quotes;
		else if (str[i] == '$' && !in_single_quotes && str[i + 1] && str[i + 1] != ' ' && str[i + 1] != '"' && str[i + 1] != '\'')
		{
			i++;
			if (str[i] == '?')  // Gestion de $?
			{
				char *exit_str = ft_itoa(minishell->exit_nb);
				if (exit_str)
				{
					strncpy(result + j, exit_str, len - j);
					j += strlen(exit_str);
					free(exit_str);
				}
				i++;
				continue;
			}
			k = 0;
			while (str[i + k] && (ft_isalnum(str[i + k]) || str[i + k] == '_'))
				k++;
			if (k > 0)
			{
				var_name = ft_substr(str, i, k);
				if (var_name)
				{
					t_env *env_var = ft_find_env_var(minishell->env, var_name);
					if (env_var)
					{
						var_value = ft_strchr(env_var->var, '=');
						if (var_value)
						{
							var_value++;  // Skip the '='
							strncpy(result + j, var_value, len - j);
							j += strlen(var_value);
						}
					}
					free(var_name);
				}
				i += k;
			}
			else
			{
				result[j++] = '$';
			}
		}
		else
		{
			result[j++] = str[i++];
		}
	}
	result[j] = '\0';
	return result;
}

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
