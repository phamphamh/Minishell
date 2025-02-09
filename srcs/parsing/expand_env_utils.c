/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/07 18:03:59 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

char	*extract_var_name(char *token, int i)
{
	int		var_len;
	char	*var_name;

	var_len = ft_strcspn(&token[i + 1], " $") + 1;
	var_name = ft_substr(token, i, var_len);
	return (var_name);
}

char	*get_env_value(char *var, t_minishell *minishell)
{
	char	*value;

	if (ft_strcmp(var, "$$") == 0)
		return (ft_itoa(getpid()));
	if (ft_strcmp(var, "$?") == 0)
		return (ft_itoa(minishell->exit_nb));

	value = getenv(var + 1);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

int	replace_variable(char *token, char *new_str, int *i, int *j, t_minishell *minishell)
{
	char	*var_name;
	char	*var_value;
	int		var_len;

	var_name = extract_var_name(token, *i);
	if (!var_name)
		return (-1);
	var_value = get_env_value(var_name, minishell);
	ft_strcpy(&new_str[*j], var_value);
	var_len = ft_strlen(var_name);
	*j += ft_strlen(var_value);
	*i += var_len;
	free(var_name);
	free(var_value);
	return (0);
}
