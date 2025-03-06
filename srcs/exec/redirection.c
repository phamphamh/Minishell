/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:25:36 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Initialise les redirections et sauvegarde les descripteurs standards
 *
 * @param saved_stdin Pointeur pour stocker le descripteur d'entrée sauvegardé
 * @param saved_stdout Pointeur pour stocker le descripteur de sortie sauvegardé
 * @param last_out Pointeur pour stocker la dernière redirection de sortie
 * @param last_in Pointeur pour stocker la dernière redirection d'entrée
 * @param last_heredoc Pointeur pour stocker la dernière redirection heredoc
 * @param redir Liste des redirections à traiter
 * @return int 1 si l'initialisation est réussie, 0 en cas d'erreur
 */
static int ft_init_redirections(int *saved_stdin, int *saved_stdout,
                              t_redirection **last_out, t_redirection **last_in,
                              t_redirection **last_heredoc, t_redirection *redir)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (*saved_stdout == -1 || *saved_stdin == -1)
		return (0);

	ft_find_last_redirections(redir, last_out, last_in, last_heredoc);
	return (1);
}

/**
 * @brief Exécute les redirections d'entrée (fichier ou heredoc)
 *
 * @param last_in Redirection d'entrée à traiter
 * @param last_heredoc Redirection heredoc à traiter
 * @param saved_stdin Descripteur d'entrée sauvegardé
 * @param saved_stdout Descripteur de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
static int ft_process_input_redirections(t_redirection *last_in,
                                      t_redirection *last_heredoc,
                                      int saved_stdin, int saved_stdout)
{
	if (last_heredoc)
	{
		return (ft_handle_heredoc(last_heredoc, saved_stdin, saved_stdout));
	}
	else if (last_in)
	{
		return (ft_handle_input(last_in, saved_stdin, saved_stdout));
	}
	return (1);
}

/**
 * @brief Gère toutes les redirections pour une commande
 *
 * @param redir Liste des redirections à traiter
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_handle_redirection(t_redirection *redir)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;
	int				result;

	if (!ft_init_redirections(&saved_stdin, &saved_stdout,
							&last_out, &last_in, &last_heredoc, redir))
		return (0);

	// Traiter les redirections d'entrée (fichier ou heredoc)
	result = ft_process_input_redirections(last_in, last_heredoc,
										saved_stdin, saved_stdout);

	// Traiter les redirections de sortie si nécessaire
	if (last_out && result)
	{
		if (!ft_handle_output(last_out, saved_stdin, saved_stdout))
			result = 0;
	}

	// Si une erreur s'est produite, restaurer les descripteurs
	if (!result)
		ft_restore_fds(saved_stdin, saved_stdout);

	ft_register_fd(STDIN_FILENO);
	ft_register_fd(STDOUT_FILENO);
	ft_register_fd(STDERR_FILENO);

	return (result);
}

/**
 * @brief Tente d'ouvrir un fichier d'entrée et enregistre le descripteur
 *
 * @param last_in Redirection d'entrée à traiter
 * @return int Descripteur de fichier ouvert, ou -1 en cas d'erreur
 */
static int ft_open_input_file(t_redirection *last_in)
{
	int fd;

	fd = open(last_in->file, O_RDONLY);
	// Enregistrer le descripteur ouvert
	if (fd != -1)
		ft_register_fd(fd);

	return (fd);
}

/**
 * @brief Gère l'affichage d'erreur pour un fichier d'entrée introuvable
 *
 * @param last_in Redirection d'entrée qui a échoué
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 */
static void ft_handle_input_error(t_redirection *last_in, int saved_stdin, int saved_stdout)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(last_in->file, 2);
	ft_putstr_fd(": No such file or directory\n", 2);
	ft_restore_fds(saved_stdin, saved_stdout);
}

/**
 * @brief Gère l'ouverture et la redirection de l'entrée
 *
 * @param last_in Redirection d'entrée à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_handle_input(t_redirection *last_in, int saved_stdin, int saved_stdout)
{
	int	fd;

	fd = ft_open_input_file(last_in);
	if (fd == -1)
	{
		ft_handle_input_error(last_in, saved_stdin, saved_stdout);
		return (0);
	}

	dup2(fd, STDIN_FILENO);
	close(fd);
	ft_unregister_fd(fd);
	return (1);
}

/**
 * @brief Tente d'ouvrir un fichier de sortie et enregistre le descripteur
 *
 * @param last_out Redirection de sortie à traiter
 * @return int Descripteur de fichier ouvert, ou -1 en cas d'erreur
 */
static int ft_open_output_file(t_redirection *last_out)
{
	int fd;

	if (last_out->type == TOKEN_REDIR_OUT)
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_APPEND, 0644);

	// Enregistrer le descripteur ouvert
	if (fd != -1)
		ft_register_fd(fd);

	return (fd);
}

/**
 * @brief Gère l'affichage d'erreur pour un fichier de sortie inaccessible
 *
 * @param last_out Redirection de sortie qui a échoué
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 */
static void ft_handle_output_error(t_redirection *last_out, int saved_stdin, int saved_stdout)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(last_out->file, 2);
	ft_putstr_fd(": Cannot open file\n", 2);
	ft_restore_fds(saved_stdin, saved_stdout);
}

/**
 * @brief Gère l'ouverture et la redirection de la sortie
 *
 * @param last_out Redirection de sortie à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_handle_output(t_redirection *last_out, int saved_stdin, int saved_stdout)
{
	int	fd;

	// Si le fichier est /dev/stdout, on utilise directement stdout
	if (ft_strcmp(last_out->file, "/dev/stdout") == 0)
	{
		// Utiliser directement le descripteur standard de sortie
		dup2(STDOUT_FILENO, STDOUT_FILENO);
		return (1);
	}

	fd = ft_open_output_file(last_out);
	if (fd == -1)
	{
		ft_handle_output_error(last_out, saved_stdin, saved_stdout);
		return (0);
	}

	dup2(fd, STDOUT_FILENO);
	close(fd);
	ft_unregister_fd(fd);
	return (1);
}

/**
 * @brief Restaure les descripteurs de fichiers standard
 *
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 */
void	ft_restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}
