/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/08 21:14:54 by tcousin          ###   ########.fr       */
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

	if (!cmd->args[1])
	{
		pwd_var = ft_find_env_var(minishell->env, "HOME");
		if (!pwd_var || !pwd_var->var)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
		path = strchr(pwd_var->var, '=') + 1;
	}
	else if (cmd->args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	else
		path = cmd->args[1];

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		ft_putstr_fd("minishell: cd: error retrieving current directory\n", 2);
		return (1);
	}

	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free(old_pwd);
		return (1);
	}

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		ft_putstr_fd("minishell: cd: error retrieving current directory\n", 2);
		free(old_pwd);
		return (1);
	}

	pwd_var = ft_find_env_var(minishell->env, "PWD");
	if (pwd_var)
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
 * @return int Code de retour (0 si succès, 1 si erreur)
 */
static int	ft_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		ft_putstr_fd("minishell: pwd: error retrieving current directory\n", 2);
		return (1);
	}
	ft_putendl_fd(pwd, 1);
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
				ret = ft_pwd();
			else if (ft_strcmp(cmd->name, "export") == 0)
			{
				if (!cmd->args[1])
					ft_print_export_list(minishell->env);
				else
					ret = ft_handle_export_var(minishell, cmd->args[1]); // Correction ici
			}
			else if (ft_strcmp(cmd->name, "unset") == 0)
			{
				if (cmd->args[1])
					ret = ft_handle_unset_var(minishell, cmd->args[1]); // Correction ici
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
		ret = ft_pwd();
	else if (ft_strcmp(cmd->name, "export") == 0)
	{
		if (!cmd->args[1])
			ft_print_export_list(minishell->env);
		else
		{
			ret = ft_handle_export_var(minishell, cmd->args[1]); // Correction ici
		}
	}
	else if (ft_strcmp(cmd->name, "unset") == 0)
	{
		if (cmd->args[1])
			ret = ft_handle_unset_var(minishell, cmd->args[1]); // Correction ici
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

