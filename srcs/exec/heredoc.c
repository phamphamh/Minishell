/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/13 11:15:52 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_read_heredoc(t_redirection *last_heredoc, int pipe_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received)
			return (0);
		if (!line)
		{
			if (isatty(STDIN_FILENO)) // Vérifie si on est en mode interactif
			{
			ft_putstr_fd("minishell: warning: here-document EOF (wanted `", 2);
			ft_putstr_fd(last_heredoc->file, 2);
			ft_putstr_fd("')\n", 2);
			break ;
			}
			break ;
		}
		if (!ft_strcmp_trim(line, last_heredoc->file))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipe_fd);
		ft_putstr_fd("\n", pipe_fd);
		free(line);
	}
	return (1);
}

int	ft_handle_heredoc(t_redirection *last_heredoc)
{
	int		heredoc_pipe[2];
	pid_t	pid;
	int		status;

	if (pipe(heredoc_pipe) == -1)
		return (ft_putstr_fd("minishell: pipe error\n", 2), 0);

	pid = fork();
	if (pid == -1)
		return (ft_putstr_fd("minishell: fork error\n", 2), close(heredoc_pipe[0]), close(heredoc_pipe[1]), 0);

	if (pid == 0) // Processus enfant : lit le here-doc
	{
		close(heredoc_pipe[0]); // Ferme la lecture du pipe dans l'enfant
		ft_heredoc_signals(); // Bloque SIGINT pendant la saisie
		if (!ft_read_heredoc(last_heredoc, heredoc_pipe[1]))
			exit(1);
		close(heredoc_pipe[1]); // Ferme l'écriture après la saisie
		exit(0);
	}

	// Processus parent
	close(heredoc_pipe[1]); // Ferme l'écriture du pipe
	waitpid(pid, &status, 0); // Attend la fin du here-doc

	if (WIFSIGNALED(status)) // Si un signal a été reçu
	{
		close(heredoc_pipe[0]);
		return (0);
	}

	// Retourne le descripteur du here-doc pour STDIN
	return (heredoc_pipe[0]);
}

