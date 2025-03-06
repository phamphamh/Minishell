/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:15:23 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:18 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"
#include <unistd.h>
#include <sys/stat.h>

// Structure pour stocker les états des descripteurs de fichiers
// Au lieu d'utiliser des variables globales, nous l'allouons localement
typedef struct s_fd_tracker {
    int open_fds[256];
    int protected_fds[256];
} t_fd_tracker;

// Fonction qui retourne un singleton (une seule instance) de la structure
static t_fd_tracker *get_fd_tracker(void)
{
    static t_fd_tracker tracker = {{0}, {0}};
    return &tracker;
}

/**
 * @brief Protège un descripteur pour qu'il ne soit pas fermé accidentellement
 *
 * @param fd Descripteur à protéger
 */
void ft_protect_fd(int fd)
{
    t_fd_tracker *tracker = get_fd_tracker();

    if (fd >= 0 && fd < 256)
        tracker->protected_fds[fd] = 1;
}

/**
 * @brief Enregistre un descripteur comme ouvert
 *
 * @param fd Descripteur à enregistrer
 */
void ft_register_fd(int fd)
{
    t_fd_tracker *tracker = get_fd_tracker();

    if (fd >= 0 && fd < 256)
        tracker->open_fds[fd] = 1;
}

/**
 * @brief Désenregistre un descripteur (quand il est fermé)
 *
 * @param fd Descripteur à désenregistrer
 */
void ft_unregister_fd(int fd)
{
    t_fd_tracker *tracker = get_fd_tracker();

    if (fd >= 0 && fd < 256)
    {
        tracker->open_fds[fd] = 0;
        tracker->protected_fds[fd] = 0;
    }
}

/**
 * @brief Ferme tous les descripteurs de fichiers ouverts
 */
void ft_close_tracked_fds(void)
{
    t_fd_tracker *tracker = get_fd_tracker();
    int i;

    for (i = 3; i < 256; i++)
    {
        if (tracker->open_fds[i] && !tracker->protected_fds[i])
        {
            close(i);
            tracker->open_fds[i] = 0;
        }
    }
}

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

	// Enregistrer les descripteurs ouverts - ils sont importants et ne doivent pas être fermés
	ft_register_fd(pipefd[0]);
	ft_register_fd(pipefd[1]);

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
    {
		close(cmd->pipe_in);
        ft_unregister_fd(cmd->pipe_in);
        cmd->pipe_in = -1;
    }
	if (cmd->pipe_out != -1)
    {
		close(cmd->pipe_out);
        ft_unregister_fd(cmd->pipe_out);
        cmd->pipe_out = -1;
    }
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
