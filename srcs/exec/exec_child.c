/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:42:55 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Vérifie si la commande est valide ou vide
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @return int 1 si la commande est valide, 0 sinon (et clean_exit appelé)
 */
static int ft_check_command_validity(t_cmd *cmd, t_minishell *minishell)
{
	if (!cmd->name || !*cmd->name)
	{
		ft_close_tracked_fds();
		ft_clean_exit(minishell, 0);
		return (0);
	}
	if (ft_strcmp(cmd->name, "\"\"") == 0 || ft_strcmp(cmd->name, "''") == 0)
	{
		ft_putstr_fd("minishell: : command not found\n", 2);
		ft_close_tracked_fds();
		ft_clean_exit(minishell, 127);
		return (0);
	}
	return (1);
}

/**
 * @brief Vérifie si la commande est un répertoire
 *
 * @param cmd_path Chemin de la commande
 * @param minishell Structure principale du shell
 * @return int 1 si ce n'est pas un répertoire, 0 sinon (et clean_exit appelé)
 */
static int ft_check_if_directory(char *cmd_path, t_minishell *minishell)
{
	struct stat	file_stat;

	if (stat(cmd_path, &file_stat) == 0)
	{
		if ((file_stat.st_mode & 0040000) == 0040000)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd_path, 2);
			ft_putstr_fd(": is a directory\n", 2);
			free(cmd_path);
			ft_close_tracked_fds();
			ft_clean_exit(minishell, 126);
			return (0);
		}
	}
	return (1);
}

/**
 * @brief Gestion de l'erreur "commande non trouvée"
 *
 * @param cmd_name Nom de la commande
 * @param minishell Structure principale du shell
 */
static void ft_handle_command_not_found(char *cmd_name, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_name, 2);
	ft_putstr_fd(": command not found\n", 2);
	ft_close_tracked_fds();
	ft_clean_exit(minishell, 127);
}

/**
 * @brief Exécute la commande avec execve
 *
 * @param cmd Structure de la commande à exécuter
 * @param cmd_path Chemin de la commande
 * @param minishell Structure principale du shell
 */
static void ft_execute_with_execve(t_cmd *cmd, char *cmd_path, t_minishell *minishell)
{
	char **env_array;

	env_array = ft_env_to_array(minishell, minishell->env);
	if (execve(cmd_path, cmd->args, env_array) == -1)
	{
		perror("minishell");
		free(cmd_path);
		ft_close_tracked_fds();
		ft_clean_exit(minishell, 126);
	}
}

/**
 * @brief Configure les pipes et redirections pour le processus enfant
 *
 * @param cmd Structure de la commande
 * @return int 1 si réussi, 0 en cas d'erreur
 */
static int ft_setup_child_io(t_cmd *cmd)
{
	ft_setup_pipes(cmd);

	// Fermer tous les descripteurs de fichiers non nécessaires
	ft_close_all_fds(NULL);

	// Gérer les redirections
	if (!ft_handle_redirection(cmd->redirs))
		return (0);

	return (1);
}

/**
 * @brief Prépare et vérifie l'exécutable
 *
 * @param cmd Structure de la commande
 * @param minishell Structure principale du shell
 * @return char* Chemin de l'exécutable ou NULL si non trouvé/erreur
 */
static char *ft_prepare_executable(t_cmd *cmd, t_minishell *minishell)
{
	char *cmd_path;

	// Vérifier si la commande est valide
	if (!ft_check_command_validity(cmd, minishell))
		return (NULL);

	// Trouver le chemin de l'exécutable
	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
	{
		ft_handle_command_not_found(cmd->name, minishell);
		return (NULL);
	}

	// Vérifier si c'est un répertoire
	if (!ft_check_if_directory(cmd_path, minishell))
		return (NULL);

	return (cmd_path);
}

/**
 * @brief Exécute une commande dans un processus enfant
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void ft_execute_child(t_cmd *cmd, t_minishell *minishell)
{
	char *cmd_path;

	// Configurer les entrées/sorties du processus
	if (!ft_setup_child_io(cmd))
	{
		ft_close_tracked_fds();
		ft_clean_exit(minishell, 1);
	}

	// Préparer l'exécutable
	cmd_path = ft_prepare_executable(cmd, minishell);
	if (!cmd_path)
		return;

	// Exécuter la commande
	ft_execute_with_execve(cmd, cmd_path, minishell);
}
