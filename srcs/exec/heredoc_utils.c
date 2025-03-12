/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:26:00 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:43:42 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Gère le processus parent pour la redirection heredoc
 *
 * @param pipe_fds Les descripteurs du pipe
 * @param status Status de sortie du processus fils
 * @param saved_fds Descripteurs sauvegardés [0] = stdin, [1] = stdout
 * @return int 1 si succès, 0 si erreur
 */
int	ft_handle_heredoc_parent(int *pipe_fds, int status,
		int *saved_fds)
{
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		close(pipe_fds[0]);
		ft_restore_fds(saved_fds[0], saved_fds[1]);
		g_signal_received = 130;
		return (0);
	}
	dup2(pipe_fds[0], STDIN_FILENO);
	close(pipe_fds[0]);
	return (1);
}

/**
 * @brief Initialise les pipes et gère les erreurs initiales
 *
 * @param pipe_fds Les descripteurs du pipe à initialiser
 * @param saved_fds Descripteurs sauvegardés à utiliser en cas d'erreur
 * @return int 0 si erreur, 1 si succès
 */
int	ft_init_heredoc_pipes(int *pipe_fds, int *saved_fds)
{
	if (pipe(pipe_fds) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		ft_restore_fds(saved_fds[0], saved_fds[1]);
		return (0);
	}
	return (1);
}

/**
 * @brief Gère l'erreur de fork pour le heredoc
 *
 * @param pipe_fds Les descripteurs du pipe à fermer
 * @param saved_fds Descripteurs sauvegardés pour restaurer
 * @return int Toujours 0
 */
int	ft_handle_fork_error(int *pipe_fds, int *saved_fds)
{
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	ft_restore_fds(saved_fds[0], saved_fds[1]);
	return (0);
}
