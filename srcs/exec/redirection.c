/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/08 00:22:36 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_open_file(char *file, int type)
{
	int	fd;

	if (type == TOKEN_REDIR_OUT)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (type == TOKEN_REDIR_APPEND)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (type == TOKEN_REDIR_IN)
		fd = open(file, O_RDONLY);
	else
		return (-1);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (-1);
	}
	return (fd);
}

int	ft_handle_redirection(t_redirection *redir)
{
	int	fd;
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	while (redir)
	{
		fd = ft_open_file(redir->file, redir->type);
		if (fd == -1)
		{
			if (saved_stdout != -1)
				dup2(saved_stdout, STDOUT_FILENO);
			if (saved_stdin != -1)
				dup2(saved_stdin, STDIN_FILENO);
			return (0);
		}
		if (redir->type == TOKEN_REDIR_IN)
			dup2(fd, STDIN_FILENO);
		else if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
			dup2(fd, STDOUT_FILENO);
		close(fd);
		redir = redir->next;
	}
	if (saved_stdout != -1)
		close(saved_stdout);
	if (saved_stdin != -1)
		close(saved_stdin);
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
