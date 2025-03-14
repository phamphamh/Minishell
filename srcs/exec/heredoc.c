/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

// Fonction pour gérer proprement la sortie des processus enfants here-doc
static void	ft_heredoc_child_exit(t_minishell *minishell, int exit_code,
		int fd_to_close)
{
	if (fd_to_close >= 0)
		close(fd_to_close);
	if (!minishell)
		exit(exit_code);
	ft_clean_exit(minishell, exit_code);
	exit(exit_code);
}

static int	ft_read_heredoc(t_redirection *last_heredoc, int pipe_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received)
		{
			free(line);
			return (0);
		}
		if (!line)
		{
			if (isatty(STDIN_FILENO))
			{
				ft_putstr_fd("minishell: warning: here-document EOF (wanted `",
					2);
				ft_putstr_fd(last_heredoc->file, 2);
				ft_putstr_fd("')\n", 2);
			}
			break ;
		}
		if (!ft_strcmp_trim(line, last_heredoc->file))
		{
			free(line);
			break ;
		}
		write(pipe_fd, line, ft_strlen(line));
		write(pipe_fd, "\n", 1);
		free(line);
	}
	return (1);
}

int	ft_handle_heredoc(t_redirection *last_heredoc, t_cmd *heredoc_cmd,
		t_minishell *minishell)
{
	int				heredoc_pipe[2];
	pid_t			pid;
	int				status;
	bool			has_command;
	t_redirection	*redir;
	t_redirection	*output_redir;
	int				fd;
	pid_t			cmd_pid;
	int				fd_out;
	int				flags;

	if (!last_heredoc || !heredoc_cmd)
		return (-1);
	has_command = heredoc_cmd->args && heredoc_cmd->args[0];
	redir = heredoc_cmd->redirs;
	output_redir = NULL;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
			output_redir = redir;
		redir = redir->next;
	}
	if (pipe(heredoc_pipe) == -1)
		return (ft_putstr_fd("minishell: pipe error\n", 2), -1);
	pid = fork();
	if (pid == -1)
		return (ft_putstr_fd("minishell: fork error\n", 2),
			close(heredoc_pipe[0]), close(heredoc_pipe[1]), -1);
	if (pid == 0)
	{
		close(heredoc_pipe[0]);
		ft_heredoc_signals();
		// Fermer tous les FDs sauf ceux dont on a besoin
		for (fd = 3; fd < 1024; fd++)
			if (fd != heredoc_pipe[1])
				close(fd);
		if (!ft_read_heredoc(last_heredoc, heredoc_pipe[1]))
		{
			close(heredoc_pipe[1]);
			ft_heredoc_child_exit(minishell, 1, -1);
		}
		close(heredoc_pipe[1]);
		ft_heredoc_child_exit(minishell, 0, -1);
	}
	close(heredoc_pipe[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		close(heredoc_pipe[0]);
		return (-1);
	}
	if (has_command && output_redir)
	{
		cmd_pid = fork();
		if (cmd_pid == -1)
			return (ft_putstr_fd("minishell: fork error\n", 2),
				close(heredoc_pipe[0]), -1);
		if (cmd_pid == 0)
		{
			dup2(heredoc_pipe[0], STDIN_FILENO);
			close(heredoc_pipe[0]);
			if (output_redir->type == TOKEN_REDIR_OUT)
				flags = O_CREAT | O_WRONLY | O_TRUNC;
			else
				flags = O_CREAT | O_WRONLY | O_APPEND;
			fd_out = open(output_redir->file, flags, 0644);
			if (fd_out == -1)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(output_redir->file, 2);
				ft_putstr_fd(": No such file or directory\n", 2);
				ft_heredoc_child_exit(minishell, 1, -1);
			}
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
			// Fermer tous les descripteurs de fichier inutiles (3-1023)
			for (fd = 3; fd < 1024; fd++)
				if (fd != STDIN_FILENO && fd != STDOUT_FILENO
					&& fd != STDERR_FILENO)
					close(fd);
			execvp(heredoc_cmd->args[0], heredoc_cmd->args);
			ft_putstr_fd("minishell: command not found\n", 2);
			ft_heredoc_child_exit(minishell, 127, -1);
		}
		close(heredoc_pipe[0]);
		waitpid(cmd_pid, &status, 0);
		return (-1);
	}
	return (heredoc_pipe[0]);
}
