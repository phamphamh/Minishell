/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:31:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/25 01:40:27 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	ft_putstr_fd("\n[LOG] ft_clean_exit: Nettoyage final et sortie\n", 2);
	ft_gc_clear(&minishell->gc_head);
	if (exit_num != -1)
		exit(exit_num);
	else
		exit(minishell->exit_nb);
}

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
	int		i;
	char	*equal_pos;

	if (!str || !*str || (!ft_isalpha(*str) && *str != '_'))
		return (false);
	equal_pos = ft_strchr(str, '=');
	if (!equal_pos)
		return (ft_is_valid_identifier(str));
	i = 1;
	while (str + i < equal_pos)
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

int	ft_env_var_match(const char *env_var, const char *var_name)
{
	size_t	len;

	if (!env_var || !var_name)
		return (0);
	len = ft_strlen(var_name);
	return (ft_strncmp(env_var, var_name, len) == 0
		&& (env_var[len] == '=' || env_var[len] == '\0'));
}

t_env	*ft_find_env_var(t_env *env, const char *var)
{
	t_env	*current;

	if (!env || !var)
		return (NULL);
	current = env;
	while (current)
	{
		if (ft_env_var_match(current->var, var))
			return (current);
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
	ft_gc_add(&minishell->gc_head, new_var);
	new_var->var = ft_strdup(var);
	if (!new_var->var)
	{
		ft_gc_remove(&minishell->gc_head, new_var);
		return ;
	}
	ft_gc_add(&minishell->gc_head, new_var->var);
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
