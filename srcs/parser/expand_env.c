/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 13:40:45 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/15 13:38:20 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static char	*prepare_result_buffer(const char *str)
{
	int		len;
	char	*result;

	len = strlen(str) * 2;
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	return (result);
}

static int	extract_var_name(const char *str, int i, char **var_name)
{
	int	k;

	k = 0;
	if (str[i] == '?')
	{
		*var_name = strdup("?");
		return (1);
	}
	while (str[i + k] && (ft_isalnum(str[i + k]) || str[i + k] == '_'))
		k++;
	*var_name = NULL;
	if (k > 0)
		*var_name = ft_substr(str, i, k);
	return (k);
}

static void	replace_var(char *result, int *j, char *var_name, t_minishell *ms)
{
	t_env	*env_var;
	char	*var_value;

	env_var = ft_find_env_var(ms->env, var_name);
	if (env_var)
	{
		var_value = ft_strchr(env_var->var, '=');
		if (var_value)
		{
			var_value++;
			strncpy(result + *j, var_value, strlen(var_value));
			*j += strlen(var_value);
		}
	}
}

static int	handle_dollar(char *res, const char *str, int *i, int *j, t_minishell *ms)
{
	char	*var_name;
	int		var_len;

	(*i)++;
	var_len = extract_var_name(str, *i, &var_name);
	if (var_len > 0)
	{
		if (strcmp(var_name, "?") == 0)
		{
			char *exit_str = ft_itoa(ms->exit_nb);
			if (exit_str)
			{
				strncpy(res + *j, exit_str, strlen(exit_str));
				*j += strlen(exit_str);
				free(exit_str);
			}
		}
		else
			replace_var(res, j, var_name, ms);
		free(var_name);
		*i += var_len;
		return (1);
	}
	res[(*j)++] = '$';
	return (0);
}

char	*expand_env_vars(const char *str, t_minishell *ms)
{
	char	*res;
	int		i;
	int		j;
	bool	in_squotes;

	res = prepare_result_buffer(str);
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
	in_squotes = false;
	while (str[i])
	{
		if (str[i] == '\'')
			in_squotes = !in_squotes;
		else if (str[i] == '$' && !in_squotes && str[i + 1] && str[i + 1] != ' ' &&
			str[i + 1] != '"' && str[i + 1] != '\'')
		{
			if (handle_dollar(res, str, &i, &j, ms))
				continue;
		}
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return (res);
}


