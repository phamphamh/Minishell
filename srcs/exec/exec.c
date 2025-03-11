/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 15:05:48 by yboumanz         ###   ########.fr       */
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
 * @brief Récupère les chemins où chercher les exécutables depuis la variable PATH
 *
 * @param env Liste des variables d'environnement
 * @return char** Tableau des chemins, NULL si PATH n'est pas défini
 */
static char	**ft_get_paths(t_env *env)
{
	char	**paths;
	char	*path_value;
	int		i;
	t_env	*current;

	// Recherche de la variable PATH dans l'environnement
	path_value = NULL;
	current = env;
	while (current)
	{
		if (ft_strncmp(current->var, "PATH=", 5) == 0)
		{
			path_value = current->var + 5;
			break;
		}
		current = current->next;
	}

	// Si PATH n'est pas trouvé ou est vide, utiliser des chemins par défaut
	if (!path_value || !*path_value)
	{
		paths = malloc(sizeof(char *) * 7);
		if (!paths)
			return (NULL);
		paths[0] = ft_strdup("/usr/local/bin");
		paths[1] = ft_strdup("/usr/bin");
		paths[2] = ft_strdup("/bin");
		paths[3] = ft_strdup("/usr/sbin");
		paths[4] = ft_strdup("/sbin");
		paths[5] = ft_strdup(".");
		paths[6] = NULL;

		// Vérifier si l'allocation a réussi pour tous les chemins
		i = 0;
		while (i < 6)
		{
			if (!paths[i])
			{
				while (i > 0)
					free(paths[--i]);
				free(paths);
				return (NULL);
			}
			i++;
		}

		return (paths);
	}

	// Découpage de la variable PATH en chemins individuels
	paths = ft_split(path_value, ':');
	if (!paths)
		return (NULL);

	// Vérification supplémentaire que les chemins sont bien extraits
	i = 0;
	while (paths[i])
		i++;

	// Si aucun chemin n'est trouvé, libérer le tableau
	if (i == 0)
	{
		free(paths);
		return (NULL);
	}

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

	// Si la commande contient déjà un chemin, l'utiliser directement
	if (!cmd_name || !*cmd_name)
		return (NULL);

	// Si la commande contient un slash, c'est déjà un chemin
	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));

	// Récupérer les chemins de recherche
	paths = ft_get_paths(env);
	if (!paths)
	{
		// Si on ne trouve pas de PATH, essayer juste la commande dans le répertoire courant
		if (access(cmd_name, X_OK) == 0)
			return (ft_strdup(cmd_name));
		return (NULL);
	}

	// Parcourir tous les chemins pour trouver la commande
	i = 0;
	while (paths[i])
	{
		// Construire le chemin complet
		temp = ft_strjoin(paths[i], "/");
		if (!temp)
		{
			ft_free_arrays(paths);
			return (NULL);
		}

		cmd_path = ft_strjoin(temp, cmd_name);
		free(temp);

		if (!cmd_path)
		{
			ft_free_arrays(paths);
			return (NULL);
		}

		// Vérifier si le fichier existe et est exécutable
		if (access(cmd_path, X_OK) == 0)
		{
			ft_free_arrays(paths);
			return (cmd_path);
		}

		free(cmd_path);
		i++;
	}

	ft_free_arrays(paths);
	// Si on arrive ici, la commande n'a pas été trouvée
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
void ft_close_unused_fds(t_cmd *cmd)
{
	int i;
	int preserve_in;
	int preserve_out;

	// Déterminer quels descripteurs préserver
	preserve_in = (cmd->pipe_in != -1) ? cmd->pipe_in : -1;
	preserve_out = (cmd->pipe_out != -1) ? cmd->pipe_out : -1;

	// Fermer tous les descripteurs de 3 à 1024, sauf ceux à préserver
	i = 3;
	while (i < 1024)
	{
		// Ne pas fermer stdin, stdout, stderr (même si c'est déjà vérifié par i >= 3)
		// Ne pas fermer les descripteurs de pipe utilisés par cette commande
		if (i != STDIN_FILENO && i != STDOUT_FILENO && i != STDERR_FILENO &&
			i != preserve_in && i != preserve_out)
		{
			close(i);
		}
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
	int			i;

	// Validation du nom de la commande
	if (!cmd->name || !*cmd->name)
	{
		ft_putstr_fd("minishell: commande vide\n", 2);
		ft_clean_exit(minishell, 0);
	}

	// Recherche du chemin de la commande
	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
	{
		if (ft_strcmp(cmd->name, "heredoc") != 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->name, 2);
			ft_putstr_fd(": command not found\n", 2);
		}
		ft_clean_exit(minishell, 127);
	}

	// Vérifier que le chemin trouvé n'est pas un répertoire
	if (stat(cmd_path, &file_stat) == 0)
	{
		if (S_ISDIR(file_stat.st_mode))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd_path, 2);
			ft_putstr_fd(": is a directory\n", 2);
			free(cmd_path);
			ft_clean_exit(minishell, 126);
		}
	}

	// Vérifier les droits d'exécution
	if (access(cmd_path, X_OK) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd_path, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		free(cmd_path);
		ft_clean_exit(minishell, 126);
	}

	// Convertir l'environnement en tableau pour execve
	env_array = ft_env_to_array(minishell, minishell->env);
	if (!env_array)
	{
		free(cmd_path);
		ft_putstr_fd("minishell: memory allocation error\n", 2);
		ft_clean_exit(minishell, 1);
	}

	// Exécuter la commande
	execve(cmd_path, cmd->args, env_array);

	// Si on arrive ici, c'est que execve a échoué
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	free(cmd_path);

	// Libérer env_array avant de quitter
	if (env_array)
	{
		i = 0;
		while (env_array[i])
		{
			free(env_array[i]);
			i++;
		}
		free(env_array);
	}

	if (errno == ENOENT) // Fichier introuvable
		ft_clean_exit(minishell, 127);
	else
		ft_clean_exit(minishell, 126); // Autres erreurs (permissions, etc.)
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

	// Validation basique
	if (!cmd || !cmd->name || !*cmd->name)
		return;

	// Créer un pipe si nécessaire
	if (cmd->next && !ft_create_pipe(cmd))
	{
		minishell->exit_nb = 1;
		return;
	}

	// Exécuter un builtin directement
	if (ft_is_builtin(cmd->name))
	{
		// Sauvegarder les descripteurs standard
		saved_stdin = dup(STDIN_FILENO);
		saved_stdout = dup(STDOUT_FILENO);

		if (saved_stdin == -1 || saved_stdout == -1)
		{
			ft_putstr_fd("minishell: dup error\n", 2);
			minishell->exit_nb = 1;
			return;
		}

		// Configurer les pipes
		ft_setup_pipes(cmd);

		// Fermer les descripteurs inutilisés même pour les builtins
		ft_close_unused_fds(cmd);

		// Gérer les redirections
		if (!ft_handle_redirection(cmd, cmd->redirs))
		{
			minishell->exit_nb = 1;
			ft_restore_fds(saved_stdin, saved_stdout);
			return;
		}

		// Exécuter le builtin
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);

		// Restaurer les descripteurs standard
		ft_restore_fds(saved_stdin, saved_stdout);

		// Fermer les pipes utilisés
		ft_close_pipes(cmd);
		return;
	}

	// Pour les commandes externes, créer un processus enfant
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return;
	}

	if (pid == 0)
	{
		// Processus enfant : rétablir les gestionnaires de signaux par défaut
		ft_reset_signals();

		// Configurer les pipes pour cette commande
		ft_setup_pipes(cmd);

		// Fermer tous les descripteurs inutilisés
		ft_close_unused_fds(cmd);

		// Gérer les redirections
		if (!ft_handle_redirection(cmd, cmd->redirs))
			ft_clean_exit(minishell, 1);

		// Exécuter la commande
		ft_execute_child(cmd, minishell);

		// On ne devrait jamais arriver ici
		exit(EXIT_FAILURE);
	}
	else
	{
		// Processus parent
		int status;

		// Ignorer les signaux pendant l'attente
		ft_ignore_signals();

		// Fermer les pipes inutilisés dans le parent
		ft_close_pipes(cmd);

		// Attendre la fin du processus enfant
		waitpid(pid, &status, 0);

		// Mettre à jour le code de sortie
		if (WIFEXITED(status))
			minishell->exit_nb = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			minishell->exit_nb = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGINT)
				ft_putstr_fd("\n", 1);
			else if (WTERMSIG(status) == SIGQUIT)
				ft_putstr_fd("Quit (core dumped)\n", 1);
		}

		// Rétablir la gestion des signaux
		ft_setup_signals();
	}
}

