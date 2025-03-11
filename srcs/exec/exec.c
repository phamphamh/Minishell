/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:32:45 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Cherche une sous-chaîne dans une chaîne
 *
 * @param haystack Chaîne dans laquelle chercher
 * @param needle Sous-chaîne à trouver
 * @return char* Pointeur vers le début de la première occurrence,
 *               NULL si non trouvée
 */
char	*ft_strstr(const char *haystack, const char *needle)
{
	size_t	i;
	size_t	j;

	if (!haystack || !needle)
		return (NULL);
	if (!*needle)
		return ((char *)haystack);
	i = 0;
	while (haystack[i])
	{
		j = 0;
		while (needle[j] && haystack[i + j] == needle[j])
			j++;
		if (!needle[j])
			return ((char *)&haystack[i]);
		i++;
	}
	return (NULL);
}

/**
 * @brief Récupère les chemins de recherche pour les commandes
 *
 * @param env Liste des variables d'environnement
 * @return char** Tableau des chemins de recherche, NULL en cas d'erreur
 */
static char	**ft_get_paths(t_env *env)
{
	t_env	*path_var;
	char	**paths;

	path_var = ft_find_env_var(env, "PATH");
	if (!path_var)
		return (NULL);
	paths = ft_split(path_var->var + 5, ':');
	return (paths);
}

/**
 * @brief Trouve le chemin complet d'un exécutable
 *
 * @param cmd_name Nom de la commande
 * @param env Liste des variables d'environnement
 * @return char* Chemin complet de l'exécutable, NULL s'il n'est pas trouvé
 */
char	*ft_find_executable(char *cmd_name, t_env *env)
{
	char	**paths;
	char	*temp;
	char	*cmd_path;
	int		i;

	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	paths = ft_get_paths(env);
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd_name);
		free(temp);
		if (access(cmd_path, X_OK) == 0)
		{
			ft_free_arrays(paths);
			return (cmd_path);
		}
		free(cmd_path);
		i++;
	}
	ft_free_arrays(paths);
	return (NULL);
}

/**
 * @brief Ferme tous les descripteurs de fichiers inutilisés
 * Cette fonction ferme tous les descripteurs de fichiers de 3 à 1024
 * pour éviter les fuites de descripteurs.
 * Dans un pipeline, seuls les descripteurs utilisés directement par la commande
 * actuelle sont conservés.
 *
 * @param cmd Structure de la commande pour connaître les pipes à préserver
 */
static void ft_close_unused_fds(t_cmd *cmd)
{
	int i;

	i = 3;
	while (i < 1024)
	{
		// Préserver les descripteurs utilisés par la commande
		if ((cmd->pipe_in != -1 && i == cmd->pipe_in) ||
			(cmd->pipe_out != -1 && i == cmd->pipe_out))
		{
			i++;
			continue;
		}

		// Ferme tous les autres descripteurs
		close(i);
		i++;
	}
}

/**
 * @brief Exécute une commande dans un processus enfant
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_child(t_cmd *cmd, t_minishell *minishell)
{
	char		*cmd_path;
	char		**env_array;
	struct stat	file_stat;

	// Les redirections et fermetures des descripteurs sont maintenant gérées dans ft_execute_command

	if (!cmd->name || !*cmd->name)
		ft_clean_exit(minishell, 0);
	if (ft_strcmp(cmd->name, "\"\"") == 0 || ft_strcmp(cmd->name, "''") == 0)
	{
		ft_putstr_fd("minishell: : command not found\n", 2);
		ft_clean_exit(minishell, 127);
	}
	if (ft_strcmp(cmd->name, "grep") == 0 && cmd->args[1]
		&& ft_strstr(cmd->args[1], "bash"))
	{
		t_env	*env_list;

		env_list = minishell->env;
		while (env_list)
		{
			if (ft_strncmp(env_list->var, "PS1=", 4) == 0)
				break ;
			env_list = env_list->next;
		}
	}
	if (cmd->name[0] == '/' || cmd->name[0] == '.')
	{
		if (stat(cmd->name, &file_stat) == 0)
		{
			if ((file_stat.st_mode & 0040000) == 0040000)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(cmd->name, 2);
				ft_putstr_fd(": is a directory\n", 2);
				ft_clean_exit(minishell, 126);
			}
		}
	}
	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
	{
		if (ft_strcmp(cmd->name,"heredoc"))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->name, 2);
			ft_putstr_fd(": command not found\n", 2);
		}
		ft_clean_exit(minishell, 127);
	}
	if (stat(cmd_path, &file_stat) == 0)
	{
		if ((file_stat.st_mode & 0040000) == 0040000)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd_path, 2);
			ft_putstr_fd(": is a directory\n", 2);
			free(cmd_path);
			ft_clean_exit(minishell, 126);
		}
	}

	env_array = ft_env_to_array(minishell, minishell->env);
	if (execve(cmd_path, cmd->args, env_array) == -1)
	{
		perror("minishell");
		free(cmd_path);

		if (errno == ENOENT) // Fichier introuvable
			ft_clean_exit(minishell, 127);
		else
			ft_clean_exit(minishell, 126); // Autres erreurs (permissions, etc.)
	}
}

/**
 * @brief Ferme les descripteurs de fichier pour toutes les commandes dans la pipeline
 *
 * @param cmd La première commande dans la pipeline
 */
void ft_close_all_pipes(t_cmd *cmd_first)
{
	t_cmd *current = cmd_first;

	while (current)
	{
		if (current->pipe_in != -1)
		{
			close(current->pipe_in);
			current->pipe_in = -1;
		}
		if (current->pipe_out != -1)
		{
			close(current->pipe_out);
			current->pipe_out = -1;
		}
		current = current->next;
	}
}

/**
 * @brief Exécute une commande (builtin ou externe)
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_command(t_cmd *cmd, t_minishell *minishell)
{
	pid_t	pid;
	int		saved_stdin;
	int		saved_stdout;

	if (!cmd->name || !*cmd->name)
		return ;
	if (cmd->next && !ft_create_pipe(cmd))
	{
		minishell->exit_nb = 1;
		return ;
	}
	if (ft_is_builtin(cmd->name))
	{
		saved_stdin = dup(STDIN_FILENO);
		saved_stdout = dup(STDOUT_FILENO);
		ft_setup_pipes(cmd);

		// Ferme les descripteurs inutilisés même pour les builtins
		ft_close_unused_fds(cmd);

		if (!ft_handle_redirection(cmd, cmd->redirs))
		{
			minishell->exit_nb = 1;
			ft_restore_fds(saved_stdin, saved_stdout);
			return;
		}
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
		ft_restore_fds(saved_stdin, saved_stdout);
		ft_close_pipes(cmd);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return ;
	}

	if (pid == 0)
	{
		// Processus enfant
		ft_reset_signals();

		// 1. Configurer les pipes
		ft_setup_pipes(cmd);

		// 2. Fermer tous les descripteurs inutilisés - méthode radicale
		ft_close_unused_fds(cmd);

		// 3. Gérer les redirections
		if (!ft_handle_redirection(cmd, cmd->redirs))
			ft_clean_exit(minishell, 1);

		// 4. Exécuter la commande
		ft_execute_child(cmd, minishell);
	}
	else
	{
		// Processus parent
		ft_ignore_signals();

		// Fermer les pipes pour cette commande dans le parent
		ft_close_pipes(cmd);

		// Si c'est la première commande de la pipeline, fermer tous les pipes
		// des commandes suivantes pour éviter des descripteurs orphelins
		if (cmd == minishell->commands)  // Vérifie si c'est la première commande
		{
			ft_close_all_pipes(cmd->next);
		}
	}
}

