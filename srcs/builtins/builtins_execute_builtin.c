/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_execute_builtin.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:44:00 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

// Prototypes des fonctions builtins
int			ft_cd(t_cmd *cmd, t_minishell *minishell);
int			ft_echo(t_cmd *cmd);
int			ft_pwd(t_minishell *minishell);
int			ft_exit(t_cmd *cmd, t_minishell *minishell);
int			ft_exec_builtin_child2(t_cmd *cmd, t_minishell *minishell, int ret);

static int	ft_exec_builtin_child(t_cmd *cmd, t_minishell *minishell)
{
	int	ret;

	ret = 0;
	if (ft_strcmp(cmd->name, "cd") == 0)
		ret = ft_cd(cmd, minishell);
	else if (ft_strcmp(cmd->name, "echo") == 0)
		ret = ft_echo(cmd);
	else if (ft_strcmp(cmd->name, "pwd") == 0)
		ret = ft_pwd(minishell);
	else if (ft_strcmp(cmd->name, "export") == 0)
	{
		if (!cmd->args[1])
			ft_print_export_list(minishell->env);
		else
			ret = ft_handle_export_var(minishell, cmd->args);
	}
	return (ft_exec_builtin_child2(cmd, minishell, ret));
}

int	ft_exec_builtin_child2(t_cmd *cmd, t_minishell *minishell, int ret)
{
	if (ft_strcmp(cmd->name, "unset") == 0)
	{
		if (cmd->args[1])
			ret = ft_handle_unset_var(minishell, cmd->args);
	}
	else if (ft_strcmp(cmd->name, "env") == 0)
	{
		ft_print_env(minishell->env);
		ret = 0;
	}
	else if (ft_strcmp(cmd->name, "exit") == 0)
		ret = ft_exit(cmd, minishell);
	return (ret);
}

int	ft_execute_builtin(t_cmd *cmd, t_minishell *minishell)
{
	int		ret;
	pid_t	pid;
	int		status;

	if (cmd->pipe_out != -1 || cmd->pipe_in != -1)
	{
		pid = fork();
		if (pid == -1)
		{
			ft_putstr_fd("minishell: fork error\n", 2);
			return (1);
		}
		if (pid == 0)
		{
			ret = ft_exec_builtin_child(cmd, minishell);
			exit(ret);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		return (1);
	}
	ret = ft_exec_builtin_child(cmd, minishell);
	return (ret);
}
