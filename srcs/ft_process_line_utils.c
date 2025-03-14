/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_process_line_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 09:53:20 by yboumanz         ###   ########.fr       */
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
	if (!ft_handle_redirection(cmd, cmd->redirs, true, minishell))
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
		ft_execute_child_process(cmd, minishell);
	else
	{
		ft_ignore_signals();
		ft_close_pipes(cmd);
	}
	return (pid);
}

int	ft_wait_child_for_pid(t_minishell *minishell, pid_t pid)
{
	int	status;

	if (pid <= 0)
		return (1);
	if (waitpid(pid, &status, 0) == -1)
		return (ft_putstr_fd("minishell: waitpid: erreur\n", 2)
			, minishell->exit_nb = 1, 1);
	if (WIFEXITED(status))
		minishell->exit_nb = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
		{
			ft_putstr_fd("\n", 1);
			minishell->exit_nb = 130;
		}
		else if (WTERMSIG(status) == SIGQUIT)
		{
			ft_putstr_fd("Quitter (core dumped)\n", 1);
			minishell->exit_nb = 131;
		}
		else
			minishell->exit_nb = 128 + WTERMSIG(status);
	}
	return (minishell->exit_nb);
}

void	ft_wait_child(pid_t pid, int *status, t_minishell *minishell
		, int last_cmd)
{
	if (pid <= 0)
		return ;
	if (waitpid(pid, status, 0) == -1)
	{
		ft_putstr_fd("minishell: waitpid: erreur\n", 2);
		minishell->exit_nb = 1;
		return ;
	}
	ft_handle_child_signal(*status, minishell, last_cmd);
}

void	ft_foreach_cmd(t_cmd *cmd, t_minishell *minishell, pid_t *last_pid)
{
	t_cmd	*current;
	int		pipe_fds[2];

	current = cmd;
	while (current)
	{
		if (current->next)
		{
			if (pipe(pipe_fds) == -1)
			{
				ft_putstr_fd("minishell: pipe error\n", 2);
				minishell->exit_nb = 1;
				return ;
			}
			current->pipe_out = pipe_fds[1];
			current->next->pipe_in = pipe_fds[0];
		}
		current->has_pipe = (current->pipe_in != -1 || current->pipe_out != -1);
		*last_pid = ft_fork_and_execute(current, minishell);
		current = current->next;
	}
}
