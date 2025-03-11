/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 16:54:17 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* Prototypes des fonctions */
void	ft_execute_cmd_with_path(t_cmd *cmd, t_minishell *minishell, char *cmd_path);
static char	**ft_validate_default_paths(char **paths);

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
 * @brief Crée un tableau de chemins par défaut
 *
 * @return char** Tableau des chemins par défaut
 */
static char	**ft_get_default_paths(void)
{
	char	**paths;

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

	return (ft_validate_default_paths(paths));
}

/**
 * @brief Vérifie que toutes les allocations des chemins par défaut ont réussi
 *
 * @param paths Tableau des chemins à vérifier
 * @return char** Tableau des chemins, NULL si une allocation a échoué
 */
static char	**ft_validate_default_paths(char **paths)
{
	int	i;

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

/**
 * @brief Recherche la valeur de la variable PATH dans l'environnement
 *
 * @param env Liste des variables d'environnement
 * @return char* Valeur de PATH, NULL si non trouvée
 */
static char	*ft_find_path_value(t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strncmp(current->var, "PATH=", 5) == 0)
			return (current->var + 5);
		current = current->next;
	}
	return (NULL);
}

/**
 * @brief Vérifie si les chemins obtenus sont valides
 *
 * @param paths Tableau des chemins à vérifier
 * @return char** Tableau des chemins, NULL si les chemins sont invalides
 */
static char	**ft_validate_paths(char **paths)
{
	int	i;

	if (!paths)
		return (NULL);

	i = 0;
	while (paths[i])
		i++;

	if (i == 0)
	{
		free(paths);
		return (NULL);
	}

	return (paths);
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

	path_value = ft_find_path_value(env);
	if (!path_value || !*path_value)
		return (ft_get_default_paths());

	paths = ft_split(path_value, ':');
	return (ft_validate_paths(paths));
}

/**
 * @brief Vérifie si le chemin existe et est exécutable
 *
 * @param cmd_name Nom de la commande
 * @param path Un chemin à tester
 * @return char* Chemin complet de l'exécutable, NULL s'il n'est pas trouvé
 */
char	*ft_check_path(char *cmd_name, char *path)
{
	char	*temp;
	char	*cmd_path;

	temp = ft_strjoin(path, "/");
	if (!temp)
		return (NULL);

	cmd_path = ft_strjoin(temp, cmd_name);
	free(temp);
	if (!cmd_path)
		return (NULL);

	if (access(cmd_path, X_OK) == 0)
		return (cmd_path);

	free(cmd_path);
	return (NULL);
}

/**
 * @brief Cherche l'exécutable dans tous les chemins de PATH
 *
 * @param cmd_name Nom de la commande
 * @param paths Tableau des chemins PATH
 * @return char* Chemin complet de l'exécutable, NULL s'il n'est pas trouvé
 */
char	*ft_search_in_paths(char *cmd_name, char **paths)
{
	char	*cmd_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		cmd_path = ft_check_path(cmd_name, paths[i]);
		if (cmd_path)
		{
			ft_free_arrays(paths);
			return (cmd_path);
		}
		i++;
	}
	ft_free_arrays(paths);
	return (NULL);
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

	if (!cmd_name || !*cmd_name)
		return (NULL);

	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));

	paths = ft_get_paths(env);
	if (!paths)
	{
		if (access(cmd_name, X_OK) == 0)
			return (ft_strdup(cmd_name));
		return (NULL);
	}

	return (ft_search_in_paths(cmd_name, paths));
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
void	ft_close_unused_fds(t_cmd *cmd)
{
	int	i;
	int	preserve_in;
	int	preserve_out;

	preserve_in = (cmd->pipe_in != -1) ? cmd->pipe_in : -1;
	preserve_out = (cmd->pipe_out != -1) ? cmd->pipe_out : -1;

	i = 3;
	while (i < 1024)
	{
		if (i != STDIN_FILENO && i != STDOUT_FILENO && i != STDERR_FILENO &&
			i != preserve_in && i != preserve_out)
			close(i);
		i++;
	}
}

/**
 * @brief Libère la mémoire de env_array et quitte proprement
 *
 * @param minishell Structure principale du shell
 * @param cmd_path Chemin de la commande
 * @param env_array Tableau des variables d'environnement
 * @param exit_code Code de sortie
 */
void	ft_handle_execve_error(t_minishell *minishell, char *cmd_path,
								char **env_array, int exit_code)
{
	int i;

	// Afficher l'erreur
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
		ft_clean_exit(minishell, exit_code); // Autres erreurs (permissions, etc.)
}

/**
 * @brief Sauvegarde les descripteurs standard
 *
 * @param saved_stdin Pointeur pour stocker le descripteur stdin
 * @param saved_stdout Pointeur pour stocker le descripteur stdout
 * @return int 0 si succès, 1 en cas d'erreur
 */
int	ft_save_std_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);

	if (*saved_stdin == -1 || *saved_stdout == -1)
	{
		ft_putstr_fd("minishell: dup error\n", 2);
		return (1);
	}
	return (0);
}

/**
 * @brief Configure les redirections pour une commande builtin
 *
 * @param cmd Structure de la commande à configurer
 * @param saved_stdin Descripteur stdin sauvegardé
 * @param saved_stdout Descripteur stdout sauvegardé
 * @return int 0 si succès, 1 en cas d'erreur
 */
int	ft_setup_builtin_redirections(t_cmd *cmd, int saved_stdin, int saved_stdout)
{
	ft_setup_pipes(cmd);
	ft_close_unused_fds(cmd);

	if (!ft_handle_redirection(cmd, cmd->redirs))
	{
		ft_restore_fds(saved_stdin, saved_stdout);
		return (1);
	}

	return (0);
}

/**
 * @brief Exécute une commande builtin avec gestion des redirections
 *
 * @param cmd Structure de la commande builtin à exécuter
 * @param minishell Structure principale du shell
 * @return int 0 si succès, autre si erreur
 */
int	ft_execute_builtin_with_redirections(t_cmd *cmd, t_minishell *minishell)
{
	int		saved_stdin;
	int		saved_stdout;
	int		ret;

	if (ft_save_std_fds(&saved_stdin, &saved_stdout))
		return (1);

	if (ft_setup_builtin_redirections(cmd, saved_stdin, saved_stdout))
		return (1);

	ret = ft_execute_builtin(cmd, minishell);
	ft_restore_fds(saved_stdin, saved_stdout);
	ft_close_pipes(cmd);

	return (ret);
}

/**
 * @brief Affiche un message d'erreur pour une commande non trouvée
 *
 * @param cmd_name Nom de la commande
 */
void	ft_print_command_not_found(char *cmd_name)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_name, 2);
	ft_putstr_fd(": command not found\n", 2);
}

/**
 * @brief Exécute une commande dans un processus enfant
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_child(t_cmd *cmd, t_minishell *minishell)
{
	char *cmd_path;

	if (!cmd->name || !*cmd->name)
	{
		ft_putstr_fd("minishell: commande vide\n", 2);
		ft_clean_exit(minishell, 0);
	}

	cmd_path = ft_find_executable(cmd->name, minishell->env);
	if (!cmd_path)
	{
		if (ft_strcmp(cmd->name, "heredoc") != 0)
			ft_print_command_not_found(cmd->name);
		ft_clean_exit(minishell, 127);
	}

	ft_execute_cmd_with_path(cmd, minishell, cmd_path);
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
 * @brief Attend la fin d'un processus enfant et met à jour le code de sortie
 *
 * @param pid PID du processus à attendre
 * @param minishell Structure principale du shell
 */
void	ft_wait_child_process(pid_t pid, t_minishell *minishell)
{
	int	status;

	// Ignorer les signaux pendant l'attente
	ft_ignore_signals();

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

/**
 * @brief Gère le processus enfant pour exécuter une commande externe
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
static void	ft_handle_child_process(t_cmd *cmd, t_minishell *minishell)
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

/**
 * @brief Exécute une commande externe via un processus enfant
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_external_command(t_cmd *cmd, t_minishell *minishell)
{
	pid_t	pid;

	// Pour les commandes externes, créer un processus enfant
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return;
	}

	if (pid == 0)
		ft_handle_child_process(cmd, minishell);
	else
	{
		// Fermer les pipes inutilisés dans le parent
		ft_close_pipes(cmd);

		// Attendre la fin du processus enfant et mettre à jour le code de sortie
		ft_wait_child_process(pid, minishell);
	}
}

/**
 * @brief Vérifie les conditions préalables à l'exécution
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @return int 0 si l'exécution peut continuer, 1 sinon
 */
static int	ft_check_cmd_prerequisites(t_cmd *cmd, t_minishell *minishell)
{
	if (!cmd || !cmd->name || !*cmd->name)
		return (1);

	if (cmd->next && !ft_create_pipe(cmd))
	{
		minishell->exit_nb = 1;
		return (1);
	}
	return (0);
}

/**
 * @brief Exécute une commande (builtin ou externe)
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_command(t_cmd *cmd, t_minishell *minishell)
{
	if (ft_check_cmd_prerequisites(cmd, minishell))
		return;

	if (ft_is_builtin(cmd->name))
	{
		minishell->exit_nb = ft_execute_builtin_with_redirections(cmd, minishell);
		return;
	}

	ft_execute_external_command(cmd, minishell);
}

/**
 * @brief Vérifie si le chemin est un répertoire
 *
 * @param cmd_path Chemin de la commande à vérifier
 * @param minishell Structure principale du shell
 */
static void	ft_check_if_directory(char *cmd_path, t_minishell *minishell)
{
	struct stat	file_stat;

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
}

/**
 * @brief Vérifie si le chemin est un répertoire ou s'il est exécutable
 *
 * @param cmd_path Chemin de la commande à vérifier
 * @param minishell Structure principale du shell
 */
void	ft_check_cmd_permissions(char *cmd_path, t_minishell *minishell)
{
	ft_check_if_directory(cmd_path, minishell);

	if (access(cmd_path, X_OK) == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd_path, 2);
		ft_putstr_fd(": Permission denied\n", 2);
		free(cmd_path);
		ft_clean_exit(minishell, 126);
	}
}

/**
 * @brief Vérifie les permissions et exécute la commande
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @param cmd_path Chemin de la commande à exécuter
 */
void	ft_execute_cmd_with_path(t_cmd *cmd, t_minishell *minishell, char *cmd_path)
{
	char	**env_array;

	ft_check_cmd_permissions(cmd_path, minishell);

	env_array = ft_env_to_array(minishell, minishell->env);
	if (!env_array)
	{
		free(cmd_path);
		ft_putstr_fd("minishell: memory allocation error\n", 2);
		ft_clean_exit(minishell, 1);
	}

	execve(cmd_path, cmd->args, env_array);

	ft_handle_execve_error(minishell, cmd_path, env_array, 126);
}

