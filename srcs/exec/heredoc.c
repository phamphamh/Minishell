/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 11:53:58 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

// Fonction pour gérer proprement la sortie des processus enfants here-doc
void	ft_heredoc_child_exit(t_minishell *minishell, int exit_code,
		int fd_to_close)
{
	if (fd_to_close >= 0)
		close(fd_to_close);
	if (!minishell)
		exit(exit_code);
	ft_clean_exit(minishell, exit_code);
	exit(exit_code);
}

static int	ft_setup_heredoc_pipe(int heredoc_pipe[2])
{
	if (pipe(heredoc_pipe) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		return (-1);
	}
	return (0);
}

static int	ft_fork_heredoc_child(t_minishell *minishell, int heredoc_pipe[2],
		t_redirection *last_heredoc)
{
	pid_t	pid;
	int		fd;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		close(heredoc_pipe[0]);
		close(heredoc_pipe[1]);
		return (-1);
	}
	if (pid == 0)
	{
		close(heredoc_pipe[0]);
		ft_heredoc_signals();
		fd = 2;
		while (++fd < 1024)
			if (fd != heredoc_pipe[1])
				close(fd);
		if (!ft_read_heredoc(last_heredoc, heredoc_pipe[1]))
			ft_heredoc_child_exit(minishell, 1, heredoc_pipe[1]);
		ft_heredoc_child_exit(minishell, 0, heredoc_pipe[1]);
	}
	close(heredoc_pipe[1]);
	return (pid);
}

static void	ft_execute_heredoc_command(t_cmd *cmd, t_redirection *output_redir,
		t_minishell *minishell, int heredoc_fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		close(heredoc_fd);
		return ;
	}
	if (pid == 0)
		ft_exec_heredoc_child(cmd, output_redir, minishell, heredoc_fd);
	close(heredoc_fd);
	waitpid(pid, &status, 0);
}

int	ft_handle_heredoc(t_redirection *last_heredoc, t_cmd *cmd,
		t_minishell *minishell)
{
	int				heredoc_pipe[2];
	pid_t			pid;
	t_redirection	*output_redir;

	if (!last_heredoc || !cmd)
		return (-1);
	if (ft_setup_heredoc_pipe(heredoc_pipe) == -1)
		return (-1);
	output_redir = ft_get_output_redir(cmd->redirs);
	pid = ft_fork_heredoc_child(minishell, heredoc_pipe, last_heredoc);
	if (pid == -1)
		return (-1);
	if (ft_wait_heredoc(pid, heredoc_pipe) == -1)
		return (-1);
	if (cmd->args && cmd->args[0] && cmd->name && ft_strcmp(cmd->name,
			"heredoc") != 0)
	{
		ft_execute_heredoc_command(cmd, output_redir, minishell,
			heredoc_pipe[0]);
		return (-1);
	}
	return (heredoc_pipe[0]);
}
