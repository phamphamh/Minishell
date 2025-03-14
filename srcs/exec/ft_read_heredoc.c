/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 10:58:18 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 11:02:13 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static void	ft_warn_heredoc_eof(t_redirection *last_heredoc)
{
	ft_putstr_fd("minishell: warning: here-document EOF (wanted `", 2);
	ft_putstr_fd(last_heredoc->file, 2);
	ft_putstr_fd("')\n", 2);
}

static int	ft_check_heredoc_end(char *line, t_redirection *last_heredoc)
{
	if (!line)
	{
		if (isatty(STDIN_FILENO))
			ft_warn_heredoc_eof(last_heredoc);
		return (1);
	}
	if (!ft_strcmp_trim(line, last_heredoc->file))
	{
		free(line);
		return (1);
	}
	return (0);
}

static void	ft_write_heredoc_line(char *line, int pipe_fd)
{
	write(pipe_fd, line, ft_strlen(line));
	write(pipe_fd, "\n", 1);
	free(line);
}

int	ft_read_heredoc(t_redirection *last_heredoc, int pipe_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received)
			return (free(line), 0);
		if (ft_check_heredoc_end(line, last_heredoc))
			break ;
		ft_write_heredoc_line(line, pipe_fd);
	}
	return (1);
}
