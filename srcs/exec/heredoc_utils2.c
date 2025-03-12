/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:38:00 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:43:42 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Affiche un message d'avertissement pour EOF dans un heredoc
 *
 * @param delimiter Le délimiteur attendu
 * @param pipe_fd Le descripteur de fichier à fermer
 * @return int Toujours 0
 */
int	ft_handle_heredoc_eof(char *delimiter, int pipe_fd)
{
	ft_putstr_fd("minishell: warning: here-document delimited ", 2);
	ft_putstr_fd("by end-of-file (wanted `", 2);
	ft_putstr_fd(delimiter, 2);
	ft_putstr_fd("')\n", 2);
	close(pipe_fd);
	return (0);
}

/**
 * @brief Lit les entrées jusqu'à rencontrer le délimiteur pour le heredoc
 *
 * @param pipe_fd Descripteur de fichier du pipe d'écriture
 * @param delimiter Délimiteur qui termine la saisie
 * @return int Toujours 0
 */
int	ft_read_heredoc(int pipe_fd, char *delimiter)
{
	char	*line;

	ft_heredoc_signals();
	while (1)
	{
		ft_putstr_fd("> ", STDOUT_FILENO);
		line = readline("");
		if (!line)
			return (ft_handle_heredoc_eof(delimiter, pipe_fd));
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			close(pipe_fd);
			return (0);
		}
		ft_putstr_fd(line, pipe_fd);
		ft_putstr_fd("\n", pipe_fd);
		free(line);
	}
	return (0);
}

/**
 * @brief Gère le processus fils pour la redirection heredoc
 *
 * @param pipe_fds Les descripteurs du pipe
 * @param heredoc Structure de redirection contenant le délimiteur
 * @return int Toujours 0
 */
int	ft_handle_heredoc_child(int *pipe_fds, t_redirection *heredoc)
{
	close(pipe_fds[0]);
	exit(ft_read_heredoc(pipe_fds[1], heredoc->file));
	return (0);
}
