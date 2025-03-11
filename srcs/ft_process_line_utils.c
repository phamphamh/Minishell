/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_process_line_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 15:09:56 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void	ft_execute_builtin_command(t_cmd *cmd, t_minishell *minishell)
{
	int	saved_stdin;
	int	saved_stdout;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
	{
		ft_putstr_fd("minishell: dup error\n", 2);
		minishell->exit_nb = 1;
		return ;
	}
	ft_setup_pipes(cmd);
	if (!ft_handle_redirection(cmd, cmd->redirs))
		minishell->exit_nb = 1;
	else
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
	ft_restore_fds(saved_stdin, saved_stdout);
	ft_close_pipes(cmd);
}

pid_t	ft_fork_and_execute(t_cmd *cmd, t_minishell *minishell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return (-1);
	}
	else if (pid == 0)
	{
		ft_reset_signals();
		ft_setup_pipes(cmd);
		ft_close_unused_fds(cmd);
		if (!ft_handle_redirection(cmd, cmd->redirs))
			ft_clean_exit(minishell, 1);
		ft_execute_child(cmd, minishell);
		exit(EXIT_FAILURE);
	}
	else
	{
		ft_ignore_signals();
		ft_close_pipes(cmd);
	}
	return (pid);
}

void	ft_wait_child(pid_t pid, int *status, t_minishell *minishell, int last)
{
	waitpid(pid, status, 0);
	if (last)
	{
		if (WIFEXITED(*status))
			minishell->exit_nb = WEXITSTATUS(*status);
		else if (WIFSIGNALED(*status))
		{
			minishell->exit_nb = 128 + WTERMSIG(*status);
			if (WTERMSIG(*status) == SIGINT)
				ft_putstr_fd("\n", 1);
			else if (WTERMSIG(*status) == SIGQUIT)
				ft_putstr_fd("Quit (core dumped)\n", 1);
		}
	}
}
