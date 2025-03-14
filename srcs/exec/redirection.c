/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
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
		t_redirection *last_out, t_redirection *last_heredoc, t_minishell *minishell)
{
	int	heredoc_fd;
	int	stdin_fd;

	heredoc_fd = -1;
	stdin_fd = -1;
	if (last_heredoc)
	{
		heredoc_fd = ft_handle_heredoc(last_heredoc, cmd, minishell);
		if (heredoc_fd == -1 && last_out)
			return (1);
		if (heredoc_fd == -1)
			return (0);
		stdin_fd = dup(STDIN_FILENO);
		if (stdin_fd == -1)
			return (ft_putstr_fd("minishell: dup error\n", 2), close(heredoc_fd), 0);
		if (dup2(heredoc_fd, STDIN_FILENO) == -1)
			return (ft_putstr_fd("minishell: dup2 error\n", 2),
				close(heredoc_fd), close(stdin_fd), 0);
		close(heredoc_fd);
	}
	if (last_in)
	{
		if (stdin_fd != -1)
		{
			if (dup2(stdin_fd, STDIN_FILENO) == -1)
				return (ft_putstr_fd("minishell: dup2 error\n", 2),
					close(stdin_fd), 0);
			close(stdin_fd);
			stdin_fd = -1;
		}
		if (!ft_handle_input(cmd, last_in))
			return (0);
	}
	if (last_out && !ft_handle_output(cmd, last_out))
	{
		if (stdin_fd != -1)
		{
			if (dup2(stdin_fd, STDIN_FILENO) == -1)
				ft_putstr_fd("minishell: dup2 error\n", 2);
			close(stdin_fd);
		}
		return (0);
	}
	if (stdin_fd != -1)
		close(stdin_fd);
	return (1);
}

int	ft_handle_redirection(t_cmd *cmd, t_redirection *redir, bool restore_after, t_minishell *minishell)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;
	int				result;

	if (!redir)
		return (1);
	saved_stdout = -1;
	saved_stdin = -1;
	ft_save_fds(&saved_stdin, &saved_stdout);
	if (saved_stdin == -1 || saved_stdout == -1)
		return (0);
	ft_find_last_redirections(redir, &last_out, &last_in, &last_heredoc);
	result = ft_apply_redirections(cmd, last_in, last_out, last_heredoc, minishell);
	if (!result)
	{
		// Toujours fermer les descripteurs sauvegardés en cas d'erreur
		close(saved_stdin);
		close(saved_stdout);
		return (0);
	}

	// Si restore_after est vrai, restaurer les descripteurs originaux
	// Sinon, simplement fermer les descripteurs sauvegardés
	if (restore_after)
		ft_restore_fds(saved_stdin, saved_stdout);
	else
	{
		close(saved_stdin);
		close(saved_stdout);
	}
	return (1);
}
