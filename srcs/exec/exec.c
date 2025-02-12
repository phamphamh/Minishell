/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/07 13:21:45 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static char	**ft_get_paths(t_env *env)
{
	t_env	*path_var;
	char	**paths;

	path_var = ft_find_env_var(env, "PATH");
	if (!path_var)
		return (NULL);
	paths = ft_split(path_var->var + 5, ':');
	return (paths);
}

char	*ft_find_executable(char *cmd_name, t_env *env)
{
	char	**paths;
	char	*temp;
	char	*cmd_path;
	int		i;

	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	paths = ft_get_paths(env);
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd_name);
		free(temp);
		if (access(cmd_path, X_OK) == 0)
		{
			ft_free_arrays(paths);
			return (cmd_path);
		}
		free(cmd_path);
		i++;
	}
	ft_free_arrays(paths);
	return (NULL);
}

static void	ft_execute_child(t_cmd *cmd, t_minishell *minishell)
{
	char	*cmd_path;
	char	**env_array;

	ft_setup_pipes(cmd);
	if (!ft_handle_redirection(cmd->redirs))
		exit(1);
	if (!cmd->name || !*cmd->name)
		exit(0);
	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->name, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	env_array = ft_env_to_array(minishell->env);
	execve(cmd_path, cmd->args, env_array);
	ft_putstr_fd("minishell: execve error\n", 2);
	exit(126);
}

void	ft_execute_command(t_cmd *cmd, t_minishell *minishell)
{
	pid_t	pid;
	int		status;

	if (!cmd->name || !*cmd->name)
		return;
	if (ft_is_builtin(cmd->name))
	{
		if (!ft_handle_redirection(cmd->redirs))
		{
			minishell->exit_nb = 1;
			return;
		}
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
		return;
	}
	if (cmd->next && !ft_create_pipe(cmd))
	{
		minishell->exit_nb = 1;
		return;
	}
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return;
	}
	if (pid == 0)
	{
		ft_reset_signals();
		ft_execute_child(cmd, minishell);
	}
	else
	{
		ft_ignore_signals();
		ft_close_pipes(cmd);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			minishell->exit_nb = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			minishell->exit_nb = 128 + WTERMSIG(status);
		ft_setup_signals();
	}
}
