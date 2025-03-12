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

static int	ft_read_heredoc(t_redirection *last_heredoc, int pipe_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document EOF (wanted `", 2);
			ft_putstr_fd(last_heredoc->file, 2);
			ft_putstr_fd("')\n", 2);
			break ;
		}
		if (g_signal_received || !ft_strcmp_trim(line, last_heredoc->file))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipe_fd);
		ft_putstr_fd("\n", pipe_fd);
		free(line);
	}
	if (g_signal_received)
		return (0);
	return (1);
}

int	ft_handle_heredoc(t_redirection *last_heredoc)
{
	int	heredoc_pipe[2];

	if (pipe(heredoc_pipe) == -1)
		return (ft_putstr_fd("minishell: pipe error\n", 2), 0);
	ft_heredoc_signals();
	g_signal_received = 0;
	if (!ft_read_heredoc(last_heredoc, heredoc_pipe[1]))
		return (close(heredoc_pipe[1]), close(heredoc_pipe[0]), 0);
	ft_setup_signals();
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);
	return (1);
}
