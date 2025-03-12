/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:26:30 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:43:43 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Gère la redirection de type heredoc
 *
 * @param cmd Commande à laquelle appliquer la redirection
 * @param heredoc Structure de redirection de type heredoc
 * @param saved_stdin Sauvegarde de l'entrée standard
 * @param saved_stdout Sauvegarde de la sortie standard
 * @return int 1 si réussi, 0 si erreur
 */
int	ft_handle_heredoc(t_cmd *cmd, t_redirection *heredoc,
		int saved_stdin, int saved_stdout)
{
	int		pipe_fds[2];
	pid_t	pid;
	int		status;
	int		saved_fds[2];

	(void)cmd;
	saved_fds[0] = saved_stdin;
	saved_fds[1] = saved_stdout;
	if (!ft_init_heredoc_pipes(pipe_fds, saved_fds))
		return (0);
	ft_ignore_signals();
	pid = fork();
	if (pid == -1)
		return (ft_handle_fork_error(pipe_fds, saved_fds));
	if (pid == 0)
		ft_handle_heredoc_child(pipe_fds, heredoc);
	close(pipe_fds[1]);
	waitpid(pid, &status, 0);
	ft_setup_signals();
	return (ft_handle_heredoc_parent(pipe_fds, status, saved_fds));
}
