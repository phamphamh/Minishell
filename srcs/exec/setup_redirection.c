/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_redirection.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 10:45:35 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 10:48:06 by tcousin          ###   ########.fr       */
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

int	ft_setup_input_redirection(t_cmd *cmd, t_redir_info *redir)
{
	if (redir->stdin_fd != -1)
	{
		if (dup2(redir->stdin_fd, STDIN_FILENO) == -1)
		{
			ft_putstr_fd("minishell: dup2 error\n", 2);
			close(redir->stdin_fd);
			return (0);
		}
		close(redir->stdin_fd);
		redir->stdin_fd = -1;
	}
	return (ft_handle_input(cmd, redir->last_in));
}

int	ft_setup_output_redirection(t_cmd *cmd, t_redir_info *redir)
{
	if (!ft_handle_output(cmd, redir->last_out))
	{
		if (redir->stdin_fd != -1)
		{
			if (dup2(redir->stdin_fd, STDIN_FILENO) == -1)
				ft_putstr_fd("minishell: dup2 error\n", 2);
			close(redir->stdin_fd);
		}
		return (0);
	}
	return (1);
}
