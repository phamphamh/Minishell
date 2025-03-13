/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/13 13:27:56 by tcousin          ###   ########.fr       */
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

int	ft_handle_heredoc(t_redirection *last_heredoc, t_cmd *heredoc_cmd)
{
	int		heredoc_pipe[2];
	int		cmd_pipe[2];
	pid_t	pid;
	int		status;
	bool	has_command = heredoc_cmd->args && heredoc_cmd->args[0];

	// Création d'un pipe pour stocker le here-doc
	if (pipe(heredoc_pipe) == -1)
		return (ft_putstr_fd("minishell: pipe error\n", 2), -1);

	pid = fork();
	if (pid == -1)
		return (ft_putstr_fd("minishell: fork error\n", 2), close(heredoc_pipe[0]), close(heredoc_pipe[1]), -1);

	if (pid == 0) // Processus enfant : lit le here-doc
	{
		close(heredoc_pipe[0]); // Ferme la lecture du pipe
		ft_heredoc_signals(); // Bloque SIGINT pendant la saisie

		// Lire le here-doc et écrire dans le pipe
		if (!ft_read_heredoc(last_heredoc, heredoc_pipe[1]))
			exit(1);
		close(heredoc_pipe[1]); // Ferme l'écriture après saisie
		exit(0);
	}

	// Processus parent : attendre la fin du here-doc
	close(heredoc_pipe[1]); // Ferme l'écriture
	waitpid(pid, &status, 0);

	if (WIFSIGNALED(status)) // Si le here-doc a été interrompu
	{
		close(heredoc_pipe[0]);
		return (-1);
	}

	// 🎯 Si le here-doc a une commande (ex: `<< eof cat`)
	if (has_command)
	{
		// Création d'un pipe pour transmettre le résultat de la commande suivante
		if (pipe(cmd_pipe) == -1)
			return (ft_putstr_fd("minishell: pipe error\n", 2), close(heredoc_pipe[0]), -1);

		pid_t cmd_pid = fork();
		if (cmd_pid == -1)
			return (ft_putstr_fd("minishell: fork error\n", 2), close(heredoc_pipe[0]), close(cmd_pipe[0]), close(cmd_pipe[1]), -1);

		if (cmd_pid == 0) // Processus enfant : Exécute la commande du here-doc
		{
			dup2(heredoc_pipe[0], STDIN_FILENO); // Redirige STDIN vers le here-doc
			close(heredoc_pipe[0]); // Ferme l'entrée du pipe

			// Si une commande suit (`|`), on redirige vers elle
			if (heredoc_cmd->next)
			{
				dup2(cmd_pipe[1], STDOUT_FILENO);
				close(cmd_pipe[0]);
				close(cmd_pipe[1]);
			}

			execvp(heredoc_cmd->args[0], heredoc_cmd->args);
			ft_putstr_fd("minishell: command not found\n", 2);
			exit(127);
		}

		// Parent : fermer les pipes inutiles
		close(heredoc_pipe[0]);
		close(cmd_pipe[1]); // Ferme l'écriture

		// Transmet le pipe en entrée à la commande suivante (`|`)
		if (heredoc_cmd->next)
			heredoc_cmd->next->pipe_in = cmd_pipe[0];
		else
			close(cmd_pipe[0]);

		// Attendre la fin du processus here-doc
		waitpid(cmd_pid, &status, 0);
		return cmd_pipe[0]; // Retourne le descripteur du pipe
	}

	// Si pas de commande, retourner le descripteur du here-doc
	return (heredoc_pipe[0]);
}
