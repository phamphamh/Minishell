/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:44:00 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Implémentation de la commande cd
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param minishell Structure principale du shell
 * @return int Code de retour (0 si succès, 1 si erreur)
 */
static int	ft_cd(t_cmd *cmd, t_minishell *minishell)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;
	t_env	*pwd_var;
	t_env	*home_var;

	// Cas 1: aucun argument = aller au $HOME
	if (!cmd->args[1])
	{
		home_var = ft_find_env_var(minishell->env, "HOME");
		if (!home_var || !ft_strchr(home_var->var, '='))
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
		path = ft_strchr(home_var->var, '=') + 1;
	}
	// Cas 2: trop d'arguments
	else if (cmd->args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	// Cas 3: argument normal
	else
		path = cmd->args[1];

	// Sauvegarder le PWD actuel (gérer le cas où il est inaccessible)
	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		// Répertoire actuel inaccessible (peut-être supprimé)
		pwd_var = ft_find_env_var(minishell->env, "PWD");
		if (pwd_var && ft_strchr(pwd_var->var, '='))
			old_pwd = ft_strdup(ft_strchr(pwd_var->var, '=') + 1);
		else
			old_pwd = ft_strdup("/");

		if (!old_pwd)
		{
			ft_putstr_fd("minishell: cd: erreur d'allocation mémoire\n", 2);
			return (1);
		}
	}

	// Tentative de changement de répertoire
	if (chdir(path) == -1)
	{
		// Si le répertoire courant est inaccessible et qu'on demande aller au home
		if ((!cmd->args[1] || ft_strcmp(path, "~") == 0) &&
			!getcwd(NULL, 0))
		{
			// Réessayer avec le HOME explicitement
			home_var = ft_find_env_var(minishell->env, "HOME");
			if (home_var && ft_strchr(home_var->var, '='))
			{
				path = ft_strchr(home_var->var, '=') + 1;
				if (chdir(path) != -1)
				{
					// Succès, mettre à jour PWD et OLDPWD
					new_pwd = getcwd(NULL, 0);
					if (new_pwd)
					{
						update_env_var(minishell->env, "OLDPWD", old_pwd, minishell);
						update_env_var(minishell->env, "PWD", new_pwd, minishell);
						free(old_pwd);
						free(new_pwd);
						return (0);
					}
				}
			}
		}

		// Échec du changement de répertoire
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free(old_pwd);
		return (1);
	}

	// Succès du changement de répertoire, mettre à jour PWD et OLDPWD
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		// Si nouveau répertoire inaccessible (rare)
		ft_putstr_fd("minishell: cd: warning: current directory cannot be determined\n", 2);
		// Essayer de construire le nouveau chemin à partir de l'ancien
		if (path[0] == '/')
			new_pwd = ft_strdup(path);  // Chemin absolu
		else
		{
			// Chemin relatif
			new_pwd = ft_strjoin(old_pwd, "/");
			if (new_pwd)
			{
				char *tmp = new_pwd;
				new_pwd = ft_strjoin(tmp, path);
				free(tmp);
			}
		}

		if (!new_pwd)
		{
			free(old_pwd);
			return (1);
		}
	}

	// Mettre à jour les variables d'environnement
	update_env_var(minishell->env, "OLDPWD", old_pwd, minishell);
	update_env_var(minishell->env, "PWD", new_pwd, minishell);

	free(old_pwd);
	free(new_pwd);
	return (0);
}

/**
 * @brief Implémentation de la commande echo
 *
 * @param cmd Structure de la commande avec ses arguments
 * @return int Code de retour (toujours 0)
 */
static int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	n_option;
	int	first_output;
	int	fd;

	i = 1;
	n_option = 0;
	first_output = 1;
	fd = STDOUT_FILENO;

	// Si on a un pipe de sortie, on l'utilise
	if (cmd->pipe_out != -1)
		fd = cmd->pipe_out;

	// Vérification des options -n valides en début de commande
	while (cmd->args[i])
	{
		if (ft_strcmp(cmd->args[i], "-n") == 0)
			n_option = 1;
		else
			break;
		i++;
	}

	// Affichage des arguments
	while (cmd->args[i])
	{
		if (!first_output)
			ft_putchar_fd(' ', fd);
		ft_putstr_fd(cmd->args[i], fd);
		first_output = 0;
		i++;
	}

	if (!n_option)
		ft_putchar_fd('\n', fd);

	return (0);
}

/**
 * @brief Implémentation de la commande pwd
 *
 * @param minishell Structure principale du shell
 * @return int Code de retour (0 si succès, 1 si erreur)
 */
static int	ft_pwd(t_minishell *minishell)
{
	char	*pwd;
	t_env	*pwd_var;

	// D'abord essayer avec getcwd
	pwd = getcwd(NULL, 0);

	// Si getcwd échoue (probablement répertoire supprimé)
	if (!pwd)
	{
		// Utiliser la variable PWD de l'environnement comme fallback
		pwd_var = ft_find_env_var(minishell->env, "PWD");
		if (pwd_var && ft_strchr(pwd_var->var, '='))
		{
			// Extraire la valeur après le '='
			char *env_pwd = ft_strchr(pwd_var->var, '=') + 1;
			ft_putendl_fd(env_pwd, STDOUT_FILENO);

			// On retourne 0 car on a pu afficher une valeur
			// (bash fait la même chose)
			return (0);
		}

		// Si la variable PWD n'est pas disponible, signaler l'erreur
		ft_putstr_fd("minishell: pwd: erreur: répertoire courant inaccessible\n", STDERR_FILENO);
		return (1);
	}

	// Afficher le chemin et libérer la mémoire
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
	return (0);
}

/**
 * @brief Implémentation de la commande exit
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param minishell Structure principale du shell
 * @return int Code de retour (0 si succès, 1 si erreur syntaxique)
 */
static int	ft_exit(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	ft_putendl_fd("exit", 2);
	if (!cmd->args[1])
		exit_code = minishell->exit_nb;
	else
	{
		if (!ft_is_all_nb(cmd->args[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd->args[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			exit_code = 2;
		}
		else
		{
			exit_code = ft_atoi(cmd->args[1]);
			if (cmd->args[2])
			{
				ft_putstr_fd("minishell: exit: too many arguments\n", 2);
				return (1);
			}
		}
	}
	ft_clean_exit(minishell, exit_code);
	return (0);
}

/**
 * @brief Exécute une commande builtin
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @return int Code de retour de la commande exécutée
 */
int	ft_execute_builtin(t_cmd *cmd, t_minishell *minishell)
{
	int ret = 0; // Initialisation du code de retour

	// Exécution dans un sous-processus si c'est un pipe
	if (cmd->pipe_out != -1 || cmd->pipe_in != -1)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			ft_putstr_fd("minishell: fork error\n", 2);
			return (1);
		}
		if (pid == 0) // Enfant
		{
			if (ft_strcmp(cmd->name, "cd") == 0)
				ret = ft_cd(cmd, minishell);
			else if (ft_strcmp(cmd->name, "echo") == 0)
				ret = ft_echo(cmd);
			else if (ft_strcmp(cmd->name, "pwd") == 0)
				ret = ft_pwd(minishell);
			else if (ft_strcmp(cmd->name, "export") == 0)
			{
				if (!cmd->args[1])
					ft_print_export_list(minishell->env);
				else
					ret = ft_handle_export_var(minishell, cmd->args); // Correction ici
			}
			else if (ft_strcmp(cmd->name, "unset") == 0)
			{
				if (cmd->args[1])
					ret = ft_handle_unset_var(minishell, cmd->args); // Correction ici
			}
			else if (ft_strcmp(cmd->name, "env") == 0)
				ft_print_env(minishell->env);
			else if (ft_strcmp(cmd->name, "exit") == 0)
				ft_exit(cmd, minishell);

			exit(ret);
		}
		int	status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status)); // Retourne le bon exit code
		return (1); // Retourne une erreur si le processus enfant ne s'est pas terminé normalement
	}

	// Exécution directe (pas de sous-processus)
	if (ft_strcmp(cmd->name, "cd") == 0)
		ret = ft_cd(cmd, minishell);
	else if (ft_strcmp(cmd->name, "echo") == 0)
		ret = ft_echo(cmd);
	else if (ft_strcmp(cmd->name, "pwd") == 0)
		ret = ft_pwd(minishell);
	else if (ft_strcmp(cmd->name, "export") == 0)
	{
		if (!cmd->args[1])
			ft_print_export_list(minishell->env);
		else
		{
			ret = ft_handle_export_var(minishell, cmd->args); // Correction ici
		}
	}
	else if (ft_strcmp(cmd->name, "unset") == 0)
	{
		if (cmd->args[1])
			ret = ft_handle_unset_var(minishell, cmd->args); // Correction ici
	}
	else if (ft_strcmp(cmd->name, "env") == 0)
	{
		ft_print_env(minishell->env);
		ret = 0;
	}
	else if (ft_strcmp(cmd->name, "exit") == 0)
		ret = ft_exit(cmd, minishell);

	return (ret);
}

