/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/10 15:40:04 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_cd(t_cmd *cmd, t_minishell *minishell)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;
	t_env	*pwd_var;

	if (!cmd->args[1])
	{
		pwd_var = ft_find_env_var(minishell->env, "HOME");
		if (!pwd_var || !pwd_var->var)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
		path = strchr(pwd_var->var, '=') + 1;
	}
	else if (cmd->args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	else
		path = cmd->args[1];

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		ft_putstr_fd("minishell: cd: error retrieving current directory\n", 2);
		return (1);
	}

	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free(old_pwd);
		return (1);
	}

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		ft_putstr_fd("minishell: cd: error retrieving current directory\n", 2);
		free(old_pwd);
		return (1);
	}

	pwd_var = ft_find_env_var(minishell->env, "PWD");
	if (pwd_var)
		update_env_var(minishell->env, "OLDPWD", old_pwd, minishell);
	update_env_var(minishell->env, "PWD", new_pwd, minishell);

	free(old_pwd);
	free(new_pwd);
	return (0);
}

static int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	j;
	int	n_option;
	int	first_arg;

	i = 1;
	n_option = 0;
	first_arg = 1;
	while (cmd->args[i])
	{
		if (ft_strncmp(cmd->args[i], "-n", 2) == 0)
		{
			j = 2;
			while (cmd->args[i][j] == 'n')
				j++;
			if (cmd->args[i][j] != '\0')
				break;
			n_option = 1;
			i++;
		}
		else
			break;
	}
	while (cmd->args[i])
	{
		if (!first_arg)
			ft_putchar_fd(' ', 1);
		ft_putstr_fd(cmd->args[i], 1);
		first_arg = 0;
		i++;
	}
	if (!n_option)
		ft_putchar_fd('\n', 1);
	return (0);
}

static int	ft_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		ft_putstr_fd("minishell: pwd: error retrieving current directory\n", 2);
		return (1);
	}
	ft_putendl_fd(pwd, 1);
	free(pwd);
	return (0);
}

static int	ft_exit(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	ft_putendl_fd("exit", 2);
	if (!cmd->args[1])
		exit_code = minishell->exit_nb;
	else
	{
		if (!ft_is_all_nb(cmd->args[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd->args[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			exit_code = 2;
		}
		else
		{
			exit_code = ft_atoi(cmd->args[1]);
			if (cmd->args[2])
			{
				ft_putstr_fd("minishell: exit: too many arguments\n", 2);
				return (1);
			}
		}
	}
	ft_clean_exit(minishell, exit_code);
	return (0);
}

int	ft_execute_builtin(t_cmd *cmd, t_minishell *minishell)
{
	if (ft_strcmp(cmd->name, "cd") == 0)
		return (ft_cd(cmd, minishell));
	else if (ft_strcmp(cmd->name, "echo") == 0)
		return (ft_echo(cmd));
	else if (ft_strcmp(cmd->name, "pwd") == 0)
		return (ft_pwd());
	else if (ft_strcmp(cmd->name, "export") == 0)
	{
		if (!cmd->args[1])
			ft_print_export_list(minishell->env);
		else
			ft_handle_export_var(minishell, cmd->args[1]);
		return (0);
	}
	else if (ft_strcmp(cmd->name, "unset") == 0)
	{
		if (cmd->args[1])
			ft_handle_unset_var(minishell, cmd->args[1]);
		return (0);
	}
	else if (ft_strcmp(cmd->name, "env") == 0)
	{
		ft_print_env(minishell->env);
		return (0);
	}
	else if (ft_strcmp(cmd->name, "exit") == 0)
		return (ft_exit(cmd, minishell));
	return (0);
}

