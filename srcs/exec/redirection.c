/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 11:10:41 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_open_file(char *file, int flags, int mode)
{
	int	fd;

	fd = open(file, flags, mode);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
	}
	return (fd);
}

static int	ft_handle_input(t_cmd *cmd, t_redirection *last_in)
{
	int	fd;

	fd = ft_open_file(last_in->file, O_RDONLY, 0644);
	if (fd == -1)
		return (0);
	dup2(fd, STDIN_FILENO);
	close(fd);
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
	}
	return (1);
}

static int	ft_handle_output(t_cmd *cmd, t_redirection *last_out)
{
	int	fd;
	int	flags;

	if (last_out->type == TOKEN_REDIR_OUT)
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	else
		flags = O_CREAT | O_WRONLY | O_APPEND;
	fd = ft_open_file(last_out->file, flags, 0644);
	if (fd == -1)
		return (0);
	if (cmd->pipe_out != -1)
	{
		close(cmd->pipe_out);
		cmd->pipe_out = -1;
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (1);
}

static int	ft_apply_redirections(t_cmd *cmd, t_redirection *last_in,
		t_redirection *last_out, t_redirection *last_heredoc)
{
	if (last_heredoc && !ft_handle_heredoc(last_heredoc))
		return (0);
	if (last_in && !ft_handle_input(cmd, last_in))
		return (0);
	if (last_out && !ft_handle_output(cmd, last_out))
		return (0);
	return (1);
}

int	ft_handle_redirection(t_cmd *cmd, t_redirection *redir)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;

	if (!redir)
		return (1);
	ft_save_fds(&saved_stdin, &saved_stdout);
	ft_find_last_redirections(redir, &last_out, &last_in, &last_heredoc);
	if (!ft_apply_redirections(cmd, last_in, last_out, last_heredoc))
	{
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	close(saved_stdin);
	close(saved_stdout);
	return (1);
}
