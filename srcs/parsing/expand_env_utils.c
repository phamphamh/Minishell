/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 15:31:47 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

char	*extract_var_name(t_minishell *minishell, char *token, int i)
{
	int		var_len;
	char	*var_name;

	var_len = ft_strcspn(&token[i + 1], " $") + 1;
	var_name = ft_substr(token, i, var_len);
	if (var_name)
		ft_gc_add(&minishell->gc_head, var_name); // ✅ Ajout au GC

	return (var_name);
}


char	*get_env_value(char *var, t_minishell *minishell)
{
	char	*value;
	char	*result;

	if (ft_strcmp(var, "$$") == 0)
		result = ft_itoa(getpid());
	else if (ft_strcmp(var, "$?") == 0)
		result = ft_itoa(minishell->exit_nb);
	else
	{
		value = getenv(var + 1);
		result = value ? ft_strdup(value) : ft_strdup("");
	}

	if (result)
		ft_gc_add(&minishell->gc_head, result); // ✅ Ajout au GC

	return (result);
}


int	replace_variable(char *token, char *new_str, int *i, int *j, t_minishell *minishell)
{
	char	*var_name;
	char	*var_value;
	int		var_len;

	var_name = extract_var_name(minishell, token, *i);
	if (!var_name)
		return (-1);
	var_value = get_env_value(var_name, minishell);
	ft_strcpy(&new_str[*j], var_value);
	var_len = ft_strlen(var_name);
	*j += ft_strlen(var_value);
	*i += var_len;
	return (0);
}
