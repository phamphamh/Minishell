/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:45:38 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

bool	ft_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str || (!ft_isalpha(*str) && *str != '_'))
		return (false);
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

bool	ft_is_valid_identifier_before_equal(const char *str)
{
	char	*equal_pos;

	if (!str || !*str || (!ft_isalpha(*str) && *str != '_'))
		return (false);
	equal_pos = ft_strchr(str, '=');
	if (!equal_pos)
		return (ft_is_valid_identifier(str));
	return (ft_is_valid_identifier(ft_substr(str, 0, equal_pos - str)));
}

int	ft_env_var_match(const char *env_var, const char *var_name)
{
	size_t	len;

	if (!env_var || !var_name)
		return (0);
	len = ft_strlen(var_name);
	return (ft_strncmp(env_var, var_name, len) == 0 && (env_var[len] == '='
			|| env_var[len] == '\0'));
}

t_env	*ft_find_env_var(t_env *env, const char *var)
{
	t_env	*current;
	size_t	len;
	char	*equal_pos;

	if (!env || !var)
		return (NULL);
	equal_pos = ft_strchr(var, '=');
	len = ft_strlen(var);
	if (equal_pos)
		len = (size_t)(equal_pos - var);
	current = env;
	while (current)
	{
		if (ft_strncmp(current->var, var, len) == 0)
		{
			if (current->var[len] == '=' || current->var[len] == '\0')
				return (current);
		}
		current = current->next;
	}
	return (NULL);
}

void	ft_add_env_var(t_minishell *minishell, const char *var)
{
	t_env	*new_var;
	t_env	*current;

	if (!minishell || !var)
		return ;
	new_var = malloc(sizeof(t_env));
	if (!new_var)
		return ;
	new_var->var = ft_strdup(var);
	if (!new_var->var)
	{
		free(new_var);
		return ;
	}
	new_var->next = NULL;
	if (!minishell->env)
		minishell->env = new_var;
	else
	{
		current = minishell->env;
		while (current->next)
			current = current->next;
		current->next = new_var;
	}
}
