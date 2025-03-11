/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_pwd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:22:52 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_pwd(t_minishell *minishell)
{
	char	*pwd;
	t_env	*env;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		env = ft_find_env_var(minishell->env, "PWD");
		if (env && ft_strchr(env->var, '='))
		{
			ft_putendl_fd(ft_strchr(env->var, '=') + 1, STDOUT_FILENO);
			return (0);
		}
		ft_putstr_fd("minishell: pwd: error: current directory ", 2);
		ft_putstr_fd("inaccessible\n", 2);
		return (1);
	}
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
	return (0);
}
