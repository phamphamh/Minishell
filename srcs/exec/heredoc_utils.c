/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 11:03:15 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 11:10:44 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_wait_heredoc(pid_t pid, int heredoc_pipe[2])
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		close(heredoc_pipe[0]);
		return (-1);
	}
	return (0);
}

t_redirection	*ft_get_output_redir(t_redirection *redir)
{
	t_redirection	*output_redir;

	output_redir = NULL;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
			output_redir = redir;
		redir = redir->next;
	}
	return (output_redir);
}

static void	clean_fds(void)
{
	int	fd;

	fd = 2;
	while (++fd < 1024)
	{
		if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
			close(fd);
	}
}

void	ft_setup_heredoc_child(int heredoc_fd, t_redirection *output_redir)
{
	int	fd_out;
	int	flags;

	dup2(heredoc_fd, STDIN_FILENO);
	close(heredoc_fd);
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
		ft_heredoc_child_exit(NULL, 1, -1);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	clean_fds();
}
