/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 14:05:07 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	handle_is_directory(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": is a directory\n", 2);
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}

void	handle_permission_denied(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": Permission denied\n", 2);
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}

void	handle_memory_error(char *cmd_path, t_minishell *minishell)
{
	free(cmd_path);
	ft_putstr_fd("minishell: memory allocation error\n", 2);
	ft_clean_exit(minishell, 1);
}

void	handle_execve_failure(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}

int	ft_check_process_status(pid_t *pids, int cmd_count,
		t_minishell *minishell, int *remaining)
{
	int	i;
	int	status;
	int	found_completed;

	found_completed = 0;
	i = -1;
	while (++i < cmd_count)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, WNOHANG) > 0)
			{
				ft_handle_child_signal(status, minishell, i == cmd_count - 1);
				pids[i] = -1;
				(*remaining)--;
				found_completed = 1;
			}
		}
	}
	return (found_completed);
}
