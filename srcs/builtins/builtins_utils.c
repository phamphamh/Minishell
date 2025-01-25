/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 13:59:09 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/25 12:09:52 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* Met à jour une variable d'environnement existante avec une nouvelle valeur */
void	update_env_var(t_env *env, const char *name, const char *new_value, t_minishell *minishell)
{
	t_env	*current;
	size_t	name_len;

	name_len = strlen(name);
	current = env;
	while (current)
	{
		if (strncmp(current->var, name, name_len) == 0 && current->var[name_len] == '=')
		{
			free(current->var);
			current->var = malloc(strlen(name) + strlen(new_value) + 2);
			if (!current->var)
			{
				perror("malloc");
				ft_gc_clear(&minishell->gc_head);
				exit(EXIT_FAILURE);
			}
			sprintf(current->var, "%s=%s", name, new_value);
			return;
		}
		current = current->next;
	}
}

/* Met à jour les variables PWD et OLDPWD après un changement de répertoire */
void	update_pwd_and_oldpwd(t_minishell *minishell)
{
	char	cwd[PATH_MAX];
	char	*oldpwd;
	
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return;
	}
	oldpwd = getenv("PWD");
	if (oldpwd)
		update_env_var(minishell->env, "OLDPWD", oldpwd, minishell);
	update_env_var(minishell->env, "PWD", cwd, minishell);
}

/* Gère la normalisation du code de sortie (modulo 256) */
void	handle_exit_nmb(t_minishell *minishell, int exit_nmb)
{
	if (exit_nmb > 255 || exit_nmb < 0)
		ft_clean_exit(minishell, exit_nmb % 256);
	else
		ft_clean_exit(minishell, exit_nmb);
}

/* Nettoie la mémoire et quitte le programme avec le code de sortie spécifié */
void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	ft_gc_clear(&minishell->gc_head);
	if (exit_num != -1)
		exit (exit_num);
	else
		exit(minishell->exit_nb);
}

/* Affiche un message d'erreur formaté sur le descripteur de fichier spécifié */
void	ft_error_msg(char *prefix, char *arg, char *suffix, int fd)
{
	if (prefix)
		ft_putstr_fd(prefix, fd);
	if (arg)
		ft_putstr_fd(arg, fd);
	if (suffix)
		ft_putstr_fd(suffix, fd);
}

/* Vérifie si un caractère est un chiffre */
static int	ft_isnum(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/* Vérifie si une chaîne ne contient que des chiffres et éventuellement un signe */
bool	ft_is_all_nb(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isnum(str[i]) && str[i] != '-' && str[i] != '+')
			return (false);
		i++;
	}
	return (true);
}

/* Vérifie si une commande est un builtin */
bool	ft_is_builtin(char *value)
{
	if (!ft_strcmp_trim("exit", value))
		return (true);
	else if (!ft_strcmp_trim("echo", value))
		return (true);
	else if (!ft_strcmp_trim("pwd", value))
		return (true);
	else if (!ft_strcmp_trim("env", value))
		return (true);
	else if (!ft_strcmp_trim("cd", value))
		return (true);
	else if (!ft_strcmp_trim("unset", value))
		return (true);
	else if (!ft_strcmp_trim("export", value))
		return (true);
	else
		return (false);
}
