/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:15:23 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/08 20:16:50 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Crée un pipe entre deux commandes
 *
 * @param cmd Commande pour laquelle créer le pipe
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_create_pipe(t_cmd *cmd)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		return (0);
	}
	cmd->pipe_out = pipefd[1];
	if (cmd->next)
		cmd->next->pipe_in = pipefd[0];
	return (1);
}

/**
 * @brief Ferme les descripteurs de fichiers des pipes
 *
 * @param cmd Commande dont les pipes doivent être fermés
 */
void	ft_close_pipes(t_cmd *cmd)
{
	if (cmd->pipe_in != -1)
		close(cmd->pipe_in);
	if (cmd->pipe_out != -1)
		close(cmd->pipe_out);
}

/**
 * @brief Configure les entrées/sorties standard pour utiliser les pipes
 *
 * @param cmd Commande pour laquelle configurer les pipes
 */
void	ft_setup_pipes(t_cmd *cmd)
{
	if (cmd->pipe_in != -1)
	{
		dup2(cmd->pipe_in, STDIN_FILENO);
		close(cmd->pipe_in);
	}
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
	}
}

