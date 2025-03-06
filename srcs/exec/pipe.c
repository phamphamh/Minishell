/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:27:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 16:20:32 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Ferme tous les descripteurs de fichiers non standard
 *
 * Cette fonction ferme tous les descripteurs de fichiers de 3 à 255
 * à l'exception de ceux passés dans le tableau d'exceptions
 *
 * @param exceptions Tableau des descripteurs à ne pas fermer (terminé par -1)
 */
void	ft_close_all_fds(int *exceptions)
{
	int	i;
	int	j;
	int	should_close;

	i = 3;
	while (i < 256)
	{
		should_close = 1;
		j = 0;
		if (exceptions)
		{
			while (exceptions[j] != -1)
			{
				if (exceptions[j] == i)
				{
					should_close = 0;
					break;
				}
				j++;
			}
		}
		if (should_close)
			close(i);
		i++;
	}
}

/**
 * @brief Ferme les descripteurs de pipe associés à une commande
 *
 * @param cmd Structure de la commande
 */
void	ft_close_pipes(t_cmd *cmd)
{
	// Fermer les descripteurs de pipe si présents
	if (cmd->pipe_in != -1)
	{
		close(cmd->pipe_in);
		cmd->pipe_in = -1;
	}
	if (cmd->pipe_out != -1)
	{
		close(cmd->pipe_out);
		cmd->pipe_out = -1;
	}
}

/**
 * @brief Crée un pipe pour une commande
 *
 * @param cmd Structure de la commande
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_create_pipe(t_cmd *cmd)
{
	int	pipefd[2];

	if (cmd->next)
	{
		if (pipe(pipefd) == -1)
		{
			perror("pipe");
			return (0);
		}
		cmd->pipe_out = pipefd[1];
		cmd->next->pipe_in = pipefd[0];
	}
	return (1);
}

/**
 * @brief Configure les entrées/sorties des pipes pour une commande
 *
 * @param cmd Structure de la commande
 */
void	ft_setup_pipes(t_cmd *cmd)
{
	// Rediriger stdin vers pipe_in si nécessaire
	if (cmd->pipe_in != -1)
	{
		dup2(cmd->pipe_in, STDIN_FILENO);
		close(cmd->pipe_in);
		cmd->pipe_in = -1;
	}

	// Rediriger stdout vers pipe_out si nécessaire
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
		cmd->pipe_out = -1;
	}
}
