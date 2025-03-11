/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 19:05:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:48:17 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_heredoc_loop(t_redirection *last_heredoc, int write_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited\
					by end-of-file (wanted `", 2);
			ft_putstr_fd(last_heredoc->file, 2);
			ft_putstr_fd("')\n", 2);
			return (0);
		}
		if (g_signal_received)
			return (free(line), 0);
		if (!ft_strcmp_trim(line, last_heredoc->file))
		{
			free(line);
			return (1);
		}
		ft_putstr_fd(line, write_fd);
		ft_putstr_fd("\n", write_fd);
		free(line);
	}
}

int	ft_handle_heredoc(t_redirection *last_heredoc,
		int saved_stdin, int saved_stdout)
{
	int		heredoc_pipe[2];
	int		ret;

	if (pipe(heredoc_pipe) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	ft_heredoc_signals();
	g_signal_received = 0;
	ret = ft_heredoc_loop(last_heredoc, heredoc_pipe[1]);
	ft_setup_signals();
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);
	return (ret);
}
