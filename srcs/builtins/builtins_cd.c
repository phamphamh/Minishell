/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_cd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:33:43 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	cd_error(char *msg, int fd)
{
	ft_putstr_fd(msg, fd);
	return (1);
}

static int	ft_cd_prepare(t_cmd *cmd, t_minishell *minishell,
				char **path, char **old_pwd)
{
	t_env	*env;

	if (!cmd->args[1])
	{
		env = ft_find_env_var(minishell->env, "HOME");
		if (!env || !ft_strchr(env->var, '='))
			return (cd_error("minishell: cd: HOME not set\n", 2));
		*path = ft_strdup(ft_strchr(env->var, '=') + 1);
	}
	else if (cmd->args[2])
		return (cd_error("minishell: cd: too many arguments\n", 2));
	else
		*path = ft_strdup(cmd->args[1]);
	*old_pwd = getcwd(NULL, 0);
	if (!*old_pwd)
	{
		env = ft_find_env_var(minishell->env, "PWD");
		if (env && ft_strchr(env->var, '='))
			*old_pwd = ft_strdup(ft_strchr(env->var, '=') + 1);
		else
			*old_pwd = ft_strdup("/");
		if (!*old_pwd)
			return (cd_error("minishell: cd: alloc error\n", 2));
	}
	return (0);
}

static int	ft_cd_post(t_minishell *minishell, char *old_pwd, char *path)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		if (path[0] == '/')
			new_pwd = ft_strdup(path);
		else
			new_pwd = ft_strjoin(old_pwd, path);
	}
	if (!new_pwd)
	{
		free(path);
		free(old_pwd);
		return (1);
	}
	update_env_var(minishell->env, "OLDPWD", old_pwd, minishell);
	update_env_var(minishell->env, "PWD", new_pwd, minishell);
	free(old_pwd);
	free(new_pwd);
	free(path);
	return (0);
}

int	ft_cd(t_cmd *cmd, t_minishell *minishell)
{
	char	*path;
	char	*old_pwd;
	int		ret;

	ret = ft_cd_prepare(cmd, minishell, &path, &old_pwd);
	if (ret)
		return (ret);
	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file\n", 2);
		free(path);
		free(old_pwd);
		return (1);
	}
	return (ft_cd_post(minishell, old_pwd, path));
}
