/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 12:32:19 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static void	check_executable(char *cmd_path, t_minishell *minishell)
{
	struct stat	file_stat;

	if (stat(cmd_path, &file_stat) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd_path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free(cmd_path);
		ft_clean_exit(minishell, 127);
	}
	if (S_ISDIR(file_stat.st_mode))
		handle_is_directory(cmd_path, minishell);
	if (access(cmd_path, X_OK) == -1)
		handle_permission_denied(cmd_path, minishell);
}

void	ft_execute_child(t_cmd *cmd, t_minishell *minishell)
{
	char	*cmd_path;
	char	**env_array;

	if (!cmd->name || !*cmd->name)
	{
		ft_putstr_fd("minishell: commande vide\n", 2);
		ft_clean_exit(minishell, 0);
	}
	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
		handle_cmd_not_found(cmd, minishell);
	check_executable(cmd_path, minishell);
	env_array = ft_env_to_array(minishell, minishell->env);
	if (!env_array)
		handle_memory_error(cmd_path, minishell);
	execve(cmd_path, cmd->args, env_array);
	handle_execve_failure(cmd_path, minishell);
}
