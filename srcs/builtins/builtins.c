/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 17:33:39 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Détermine le chemin à utiliser pour la commande cd
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param minishell Structure principale du shell
 * @return char* Chemin à utiliser, NULL en cas d'erreur
 */
static char	*ft_get_cd_path(t_cmd *cmd, t_minishell *minishell)
{
	t_env	*home_var;
	char	*path;

	if (!cmd->args[1])
	{
		home_var = ft_find_env_var(minishell->env, "HOME");
		if (!home_var || !ft_strchr(home_var->var, '='))
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (NULL);
		}
		path = ft_strchr(home_var->var, '=') + 1;
	}
	else if (cmd->args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (NULL);
	}
	else
		path = cmd->args[1];
	return (path);
}

/**
 * @brief Gère l'échec de getcwd dans cd
 *
 * @param path Chemin cible du cd
 * @param old_pwd Ancien répertoire de travail
 * @return char* Nouveau répertoire de travail, NULL en cas d'erreur
 */
static char	*ft_handle_getcwd_failure(char *path, char *old_pwd)
{
	char	*new_pwd;

	if (path[0] == '/')
		new_pwd = ft_strdup(path);
	else
	{
		new_pwd = ft_strjoin(old_pwd, "/");
		if (new_pwd)
		{
			char *tmp = new_pwd;
			new_pwd = ft_strjoin(tmp, path);
			free(tmp);
		}
	}
	return (new_pwd);
}

/**
 * @brief Gère les erreurs lors du changement de répertoire
 *
 * @param path Chemin vers lequel changer
 * @param old_pwd Ancien répertoire à libérer en cas d'erreur
 * @return int 1 si erreur, 0 si succès
 */
static int	ft_handle_chdir_error(char *path, char *old_pwd)
{
	ft_putstr_fd("minishell: cd: ", 2);
	ft_putstr_fd(path, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
	free(old_pwd);
	return (1);
}

/**
 * @brief Obtient le répertoire courant ou une valeur par défaut
 *
 * @param minishell Structure principale du shell
 * @return char* Chemin du répertoire courant, NULL en cas d'erreur
 */
static char	*ft_get_current_dir(t_minishell *minishell)
{
	char	*pwd;
	t_env	*pwd_var;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		pwd_var = ft_find_env_var(minishell->env, "PWD");
		if (pwd_var && ft_strchr(pwd_var->var, '='))
			pwd = ft_strdup(ft_strchr(pwd_var->var, '=') + 1);
		else
			pwd = ft_strdup("/");
	}
	return (pwd);
}

/**
 * @brief Met à jour les variables d'environnement PWD et OLDPWD
 *
 * @param minishell Structure principale du shell
 * @param old_pwd Ancien répertoire de travail
 * @param new_pwd Nouveau répertoire de travail
 */
static void	ft_update_pwd_vars(t_minishell *minishell, char *old_pwd, char *new_pwd)
{
	update_env_var(minishell->env, "OLDPWD", old_pwd, minishell);
	update_env_var(minishell->env, "PWD", new_pwd, minishell);

	free(old_pwd);
	free(new_pwd);
}

/**
 * @brief Obtient le nouveau répertoire de travail après chdir
 *
 * @param path Chemin cible du cd
 * @param old_pwd Ancien répertoire de travail
 * @return char* Nouveau répertoire de travail, NULL en cas d'erreur
 */
static char	*ft_get_new_dir(char *path, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		new_pwd = ft_handle_getcwd_failure(path, old_pwd);

	return (new_pwd);
}

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

	path = ft_get_cd_path(cmd, minishell);
	if (!path)
		return (1);

	old_pwd = ft_get_current_dir(minishell);
	if (!old_pwd)
		return (1);

	if (chdir(path) != 0)
		return (ft_handle_chdir_error(path, old_pwd));

	new_pwd = ft_get_new_dir(path, old_pwd);
	if (!new_pwd)
	{
		free(old_pwd);
		return (1);
	}

	ft_update_pwd_vars(minishell, old_pwd, new_pwd);
	return (0);
}

/**
 * @brief Vérifie les options de la commande echo
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param i Pointeur vers l'index actuel
 * @return int 1 si l'option -n est présente, 0 sinon
 */
static int	ft_check_echo_options(t_cmd *cmd, int *i)
{
	int	n_option;

	n_option = 0;
	// Vérification des options -n valides en début de commande
	while (cmd->args[*i])
	{
		if (ft_strcmp(cmd->args[*i], "-n") == 0)
			n_option = 1;
		else
			break;
		(*i)++;
	}
	return (n_option);
}

/**
 * @brief Affiche les arguments de la commande echo
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param i Index du premier argument à afficher
 * @param fd Descripteur de fichier où écrire
 */
static void	ft_display_echo_args(t_cmd *cmd, int i, int fd)
{
	int	first_output;

	first_output = 1;

	// Affichage des arguments
	while (cmd->args[i])
	{
		if (!first_output)
			ft_putchar_fd(' ', fd);
		ft_putstr_fd(cmd->args[i], fd);
		first_output = 0;
		i++;
	}
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
	int	fd;

	i = 1;
	fd = STDOUT_FILENO;

	// Si on a un pipe de sortie, on l'utilise
	if (cmd->pipe_out != -1)
		fd = cmd->pipe_out;

	n_option = ft_check_echo_options(cmd, &i);

	ft_display_echo_args(cmd, i, fd);

	if (!n_option)
		ft_putchar_fd('\n', fd);

	return (0);
}

/**
 * @brief Affiche la valeur de PWD depuis la variable d'environnement
 *
 * @param pwd_var Variable d'environnement PWD
 * @return int 0 si succès
 */
static int	ft_display_env_pwd(t_env *pwd_var)
{
	char *env_pwd;

	// Extraire la valeur après le '='
	env_pwd = ft_strchr(pwd_var->var, '=') + 1;
	ft_putendl_fd(env_pwd, STDOUT_FILENO);

	// On retourne 0 car on a pu afficher une valeur
	// (bash fait la même chose)
	return (0);
}

/**
 * @brief Gère le cas où getcwd échoue dans pwd
 *
 * @param minishell Structure principale du shell
 * @return int Code de retour (0 si succès, 1 si erreur)
 */
static int	ft_handle_pwd_getcwd_failure(t_minishell *minishell)
{
	t_env	*pwd_var;

	// Utiliser la variable PWD de l'environnement comme fallback
	pwd_var = ft_find_env_var(minishell->env, "PWD");
	if (pwd_var && ft_strchr(pwd_var->var, '='))
		return (ft_display_env_pwd(pwd_var));

	// Si la variable PWD n'est pas disponible, signaler l'erreur
	ft_putstr_fd("minishell: pwd: erreur: répertoire courant inaccessible\n", STDERR_FILENO);
	return (1);
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

	// D'abord essayer avec getcwd
	pwd = getcwd(NULL, 0);

	// Si getcwd échoue (probablement répertoire supprimé)
	if (!pwd)
		return (ft_handle_pwd_getcwd_failure(minishell));

	// Afficher le chemin et libérer la mémoire
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
	return (0);
}

/**
 * @brief Détermine le code de sortie pour la commande exit
 *
 * @param cmd Structure de la commande avec ses arguments
 * @param minishell Structure principale du shell
 * @return int Code de sortie à utiliser
 */
static int	ft_get_exit_code(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	if (!cmd->args[1])
		return (minishell->exit_nb);

	if (!ft_is_all_nb(cmd->args[1]))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		return (2);
	}

	exit_code = ft_atoi(cmd->args[1]);
	if (cmd->args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (-1); // Code spécial pour indiquer qu'il ne faut pas quitter
	}

	return (exit_code);
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
	exit_code = ft_get_exit_code(cmd, minishell);

	if (exit_code == -1)
		return (1); // Erreur : trop d'arguments

	ft_clean_exit(minishell, exit_code);
	return (0); // Jamais atteint en réalité
}

/**
 * @brief Wrapper pour ft_handle_export_var pour traiter tous les arguments
 *
 * @param minishell Structure principale du shell
 * @param args Arguments de la commande export
 * @return int Code de retour
 */
static int	ft_handle_export_cmd(t_minishell *minishell, char **args)
{
	int i = 1;

	if (!args[i])
	{
		ft_print_export_list(minishell->env);
		return (0);
	}

	while (args[i])
	{
		ft_handle_export_var(minishell, args[i]);
		i++;
	}

	// Pour maintenir la compatibilité, on retourne toujours 0
	return (0);
}

/**
 * @brief Wrapper pour ft_handle_unset_var pour traiter tous les arguments
 *
 * @param minishell Structure principale du shell
 * @param args Arguments de la commande unset
 * @return int Code de retour
 */
static int	ft_handle_unset_cmd(t_minishell *minishell, char **args)
{
	int i = 1;

	while (args[i])
	{
		ft_handle_unset_var(minishell, args[i]);
		i++;
	}

	// Pour maintenir la compatibilité, on retourne toujours 0
	return (0);
}

/**
 * @brief Exécute une commande builtin spécifique
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @return int Code de retour de la commande exécutée
 */
static int	ft_execute_specific_builtin(t_cmd *cmd, t_minishell *minishell)
{
	int ret = 0;

	if (ft_strcmp(cmd->name, "cd") == 0)
		ret = ft_cd(cmd, minishell);
	else if (ft_strcmp(cmd->name, "echo") == 0)
		ret = ft_echo(cmd);
	else if (ft_strcmp(cmd->name, "pwd") == 0)
		ret = ft_pwd(minishell);
	else if (ft_strcmp(cmd->name, "export") == 0)
		ret = ft_handle_export_cmd(minishell, cmd->args);
	else if (ft_strcmp(cmd->name, "unset") == 0)
		ret = ft_handle_unset_cmd(minishell, cmd->args);
	else if (ft_strcmp(cmd->name, "env") == 0)
	{
		ft_print_env(minishell->env);
		ret = 0;
	}
	else if (ft_strcmp(cmd->name, "exit") == 0)
		ret = ft_exit(cmd, minishell);

	return (ret);
}

/**
 * @brief Exécute une commande builtin dans un processus enfant
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 * @return int Code de retour de la commande exécutée
 */
static int	ft_execute_builtin_in_child(t_cmd *cmd, t_minishell *minishell)
{
	int ret = 0;
	int status;
	pid_t pid = fork();

	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		return (1);
	}
	if (pid == 0)
	{
		ret = ft_execute_specific_builtin(cmd, minishell);
		exit(ret);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
		else
			ret = 1;
	}
	return (ret);
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
	int ret = 0;

	if (cmd->pipe_out != -1 || cmd->pipe_in != -1)
		return (ft_execute_builtin_in_child(cmd, minishell));
	else
		ret = ft_execute_specific_builtin(cmd, minishell);

	return (ret);
}

