/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/08 13:51:29 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	extract_var_name(const char *str, int i, char **var_name)
{
	int	k;

	k = 0;
	if (str[i] == '?')
	{
		*var_name = ft_strdup("?");
		return (1);
	}
	while (str[i + k] && (ft_isalnum(str[i + k]) || str[i + k] == '_'))
		k++;
	if (k > 0)
		*var_name = ft_substr(str, i, k);
	return (k);
}

static void	replace_var(char *var_name, t_expand_env *env)
{
	t_env	*env_var;
	char	*var_value;
	int		new_size;
	char	*new_res;

	env_var = ft_find_env_var(env->ms->env, var_name);
	if (!env_var)
		return ;
	var_value = ft_strchr(env_var->var, '=');
	if (!var_value || !var_value[1])
		return ;
	var_value++;
	new_size = (*env->j) + ft_strlen(var_value) + 1;
	if (new_size > env->buf_size)
	{
		new_res = realloc(env->res, new_size);
		if (!new_res)
			return ;
		env->res = new_res;
		env->buf_size = new_size;
	}
	ft_strlcpy(env->res + *env->j, var_value, ft_strlen(var_value) + 1);
	*env->j += ft_strlen(var_value);
}

static void	handle_dollar(const char *str, int *index, t_expand_env *env)
{
	char	*var_name;
	int		var_len;
	char	*exit_str;

	(*index)++;
	var_len = extract_var_name(str, *index, &var_name);
	if (var_len == 0)
	{
		env->res[(*env->j)++] = '$';
		return ;
	}
	if (ft_strcmp(var_name, "?") == 0)
	{
		exit_str = ft_itoa(env->ms->exit_nb);
		if (exit_str)
		{
			ft_strlcpy(env->res + *env->j, exit_str, ft_strlen(exit_str) + 1);
			*env->j += ft_strlen(exit_str);
			free(exit_str);
		}
	}
	else
		replace_var(var_name, env);
	free(var_name);
	*index += var_len;
}

/**
 * @brief Traite l'expansion des variables et les séquences d'échappement.
 *
 * @param str La chaîne d'entrée.
 * @param env Structure contenant le buffer et minishell.
 */
static void	process_expansion(const char *str, t_expand_env *env,
		bool in_quotes)
{
	int		i;
	bool	in_squotes;

	i = 0;
	in_squotes = false;
	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1] && !in_quotes)
			process_escape_sequence(str, env->res, &i, env->j);
		else if (str[i] == '$' && !in_squotes)
			handle_dollar(str, &i, env);
		else
			env->res[(*env->j)++] = str[i++];
	}
}

/**
 * @brief Expande les variables d'environnement dans une chaîne.
 *
 * @param str La chaîne d'entrée contenant potentiellement des variables.
 * @param ms Structure principale du shell.
 * @return char* Une nouvelle chaîne avec les variables expandées.
 */
char	*expand_env_vars(const char *str, t_minishell *ms, bool in_quotes)
{
	int				j;
	t_expand_env	env;

	j = 0;
	env.res = prepare_result_buffer(str, ms);
	if (!env.res)
		return (NULL);
	env.j = &j;
	env.ms = ms;
	env.buf_size = ft_strlen(env.res) + 1;
	process_expansion(str, &env, in_quotes);
	env.res[j] = '\0';
	return (env.res);
}
