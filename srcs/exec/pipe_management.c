/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:36:57 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"
#include <unistd.h>
#include <sys/stat.h>

/**
 * @brief Ferme tous les descripteurs de fichiers ouverts sauf ceux spécifiés
 *
 * @param exceptions Tableau des descripteurs à ne pas fermer (terminé par -1)
 */
void	ft_close_all_fds(int *exceptions)
{
	int	i;
	int	j;
	int	should_close;

	// Commencer à 3 pour éviter de fermer STDIN(0), STDOUT(1) et STDERR(2)
	i = 3;
	while (i < 256)
	{
		should_close = 1;

		// Vérifier si le descripteur est dans les exceptions
		if (exceptions)
		{
			j = 0;
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

		// Tentative de fermeture du descripteur
		if (should_close)
		{
			close(i);
		}
		i++;
	}
}

/**
 * @brief Ferme les descripteurs non utilisés lors de l'exécution d'une commande
 *
 * Cette fonction est plus ciblée pour les processus enfants
 *
 * @param cmd Commande courante
 * @param cmd_list Liste complète des commandes
 */
void ft_close_unused_pipes(t_cmd *cmd, t_cmd *cmd_list)
{
    t_cmd *current;

    current = cmd_list;
    while (current)
    {
        // Ne fermer que les descripteurs qui appartiennent à des commandes
        // sans aucun lien avec la commande actuelle et qui ne sont pas utilisés ailleurs

        // Pour les processus enfants, on ne garde que les descripteurs pertinents
        if (current != cmd)
        {
            // On ne ferme les descripteurs que s'ils ne sont pas utilisés par la commande courante
            // ou une commande directement adjacente
            if (current->pipe_out != -1 && current->pipe_out != cmd->pipe_in &&
                (cmd->next == NULL || current->pipe_out != cmd->next->pipe_in))
            {
                close(current->pipe_out);
                ft_unregister_fd(current->pipe_out);
                current->pipe_out = -1;
            }

            // Pour le pipe_in, on est plus prudent (ne pas fermer les entrées utilisées)
            if (current->pipe_in != -1 && current->pipe_in != cmd->pipe_in &&
                current->pipe_in != cmd->pipe_out)
            {
                close(current->pipe_in);
                ft_unregister_fd(current->pipe_in);
                current->pipe_in = -1;
            }
        }

        current = current->next;
    }
}
