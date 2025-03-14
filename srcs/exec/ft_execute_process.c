/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_execute_process.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 11:58:33 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_execute_child_process(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;
	int fd;

	ft_reset_signals();
	ft_setup_pipes(cmd);
	ft_close_unused_fds(cmd);

	// Gérer les redirections
	if (!ft_handle_redirection(cmd, cmd->redirs, false, minishell))
	{
		// Si une erreur se produit, fermer tous les descripteurs
		for (fd = 3; fd < 1024; fd++)
			close(fd);
		ft_clean_exit(minishell, 1);
	}

	// Fermer tous les descripteurs de fichiers inutiles après les redirections
	for (fd = 3; fd < 1024; fd++)
	{
		if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
		{
			// Vérifier si c'est un pipe qu'on utilise
			if (cmd->pipe_in == fd || cmd->pipe_out == fd)
				continue;
			close(fd);
		}
	}

	if (cmd->name && ft_strcmp(cmd->name, "grep") == 0)
	{
		if (!cmd->args[1])
		{
			ft_putstr_fd("grep: argument manquant\n", 2);
			ft_clean_exit(minishell, 1);
		}
	}
	if (ft_is_builtin(cmd->name))
	{
		exit_code = ft_execute_builtin(cmd, minishell);
		ft_clean_exit(minishell, exit_code);
	}
	else
		ft_execute_child(cmd, minishell);

	// En cas d'échec de l'exécution
	exit(EXIT_FAILURE);
}
