/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 13:32:24 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

char	**ft_get_paths(t_env *env)
{
	char	*path_value;
	t_env	*current;

	path_value = NULL;
	current = env;
	while (current)
	{
		if (ft_strncmp(current->var, "PATH=", 5) == 0)
		{
			path_value = current->var + 5;
			break ;
		}
		current = current->next;
	}
	if (!path_value || !*path_value)
		return (NULL);
	return (ft_allocate_paths(path_value));
}

char	*ft_find_cmd_in_paths(char **paths, char *cmd_name)
{
	char	*temp;
	char	*cmd_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		if (!temp)
			return (NULL);
		cmd_path = ft_strjoin(temp, cmd_name);
		free(temp);
		if (!cmd_path)
			return (NULL);
		if (access(cmd_path, X_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	return (NULL);
}

char	*ft_find_executable(char *cmd_name, t_env *env)
{
	char	**paths;
	char	*cmd_path;

	if (!cmd_name || !*cmd_name)
		return (NULL);
	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	paths = ft_get_paths(env);
	if (!paths)
	{
		if (access(cmd_name, X_OK) == 0)
			return (ft_strdup(cmd_name));
		return (NULL);
	}
	cmd_path = ft_find_cmd_in_paths(paths, cmd_name);
	ft_free_arrays(paths);
	return (cmd_path);
}

void	ft_close_unused_fds(t_cmd *cmd)
{
	int	i;
	int	preserve_in;
	int	preserve_out;

	preserve_in = -1;
	if (cmd->pipe_in != -1)
		preserve_in = cmd->pipe_in;
	preserve_out = -1;
	if (cmd->pipe_out != -1)
		preserve_out = cmd->pipe_out;
	i = 3;
	while (i < 1024)
	{
		if (i != STDIN_FILENO && i != STDOUT_FILENO && i != STDERR_FILENO)
		{
			if (i != preserve_in && i != preserve_out)
				close(i);
		}
		i++;
	}
}

void	handle_cmd_not_found(t_cmd *cmd, t_minishell *minishell)
{
	if (ft_strcmp(cmd->name, "heredoc") != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->name, 2);
		ft_putstr_fd(": command not found\n", 2);
		ft_clean_exit(minishell, 127);
	}
	ft_clean_exit(minishell, 0);
}
