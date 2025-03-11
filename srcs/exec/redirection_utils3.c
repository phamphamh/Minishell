/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils3.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 19:05:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:29:33 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_open_input(char *file, int saved_stdin, int saved_stdout)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (-1);
	}
	return (fd);
}

int	ft_handle_input(t_cmd *cmd, t_redirection *last_in,
		int saved_stdin, int saved_stdout)
{
	int	fd;

	fd = ft_open_input(last_in->file, saved_stdin, saved_stdout);
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

int	ft_handle_output(t_cmd *cmd, t_redirection *last_out,
		int saved_stdin, int saved_stdout)
{
	int	fd;

	if (last_out->type == TOKEN_REDIR_OUT)
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_out->file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	if (cmd->pipe_out != -1)
	{
		close(cmd->pipe_out);
		cmd->pipe_out = -1;
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (1);
}

void	ft_restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}
