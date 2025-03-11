/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:47:43 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:07:53 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static void	ft_update_or_add_env_var(t_minishell *minishell, char *var,
		char *var_name)
{
	t_env	*env_entry;

	env_entry = ft_find_env_var(minishell->env, var_name);
	if (env_entry)
	{
		free(env_entry->var);
		env_entry->var = ft_strdup(var);
	}
	else
		ft_add_env_var(minishell, var);
}

int	ft_handle_unset_var(t_minishell *minishell, char **var_names)
{
	int	i;
	int	ret;

	if (!minishell || !var_names || !var_names[1])
		return (0);
	i = 1;
	ret = 0;
	while (var_names[i])
	{
		if (!ft_is_valid_identifier(var_names[i]))
		{
			ft_unset_error(var_names[i], minishell);
			ret = 1;
		}
		else
			ft_remove_env_var(minishell, ft_find_env_var(minishell->env,
					var_names[i]));
		i++;
	}
	return (ret);
}

int	ft_handle_export_var(t_minishell *minishell, char **vars)
{
	int		i;
	int		ret;
	char	*var_name;
	char	*equal_pos;

	if (!minishell || !vars || !vars[1])
		return (0);
	i = 1;
	ret = 0;
	while (vars[i])
	{
		if (ft_validate_export_var(minishell, vars[i], &var_name, &equal_pos))
		{
			ret = 1;
		}
		else
		{
			ft_update_or_add_env_var(minishell, vars[i], var_name);
			free(var_name);
		}
		i++;
	}
	return (ret);
}

void	ft_export_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
}

void	ft_unset_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
}
