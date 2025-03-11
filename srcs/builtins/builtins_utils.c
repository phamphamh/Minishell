/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 13:59:09 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/09 13:36:20 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**

	* @brief Met à jour une variable d'environnement existante avec une nouvelle valeur
 *
 * @param env Liste des variables d'environnement
 * @param name Nom de la variable à mettre à jour
 * @param new_value Nouvelle valeur
 * @param minishell Structure principale du shell
 */
void	update_env_var(t_env *env, const char *name, const char *new_value,
		t_minishell *minishell)
{
	t_env	*current;
	size_t	name_len;

	name_len = strlen(name);
	current = env;
	while (current)
	{
		if (strncmp(current->var, name, name_len) == 0
			&& current->var[name_len] == '=')
		{
			current->var = malloc(strlen(name) + strlen(new_value) + 2);
			if (!current->var)
			{
				perror("malloc");
				ft_gc_clear(&minishell->gc_head);
				exit(EXIT_FAILURE);
			}
			ft_gc_add(&minishell->gc_head, current->var);
			sprintf(current->var, "%s=%s", name, new_value);
			return ;
		}
		current = current->next;
	}
}

/**

	* @brief Met à jour les variables PWD et OLDPWD après un changement de répertoire
 *
 * @param minishell Structure principale du shell
 */
void	update_pwd_and_oldpwd(t_minishell *minishell)
{
	char	cwd[PATH_MAX];
	t_env	*pwd_var;
	char	*pwd_value;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return ;
	}
	pwd_var = ft_find_env_var(minishell->env, "PWD");
	if (pwd_var)
	{
		pwd_value = ft_strchr(pwd_var->var, '=');
		if (pwd_value)
		{
			pwd_value++;
			update_env_var(minishell->env, "OLDPWD", pwd_value, minishell);
		}
	}
	update_env_var(minishell->env, "PWD", cwd, minishell);
}

/**
 * @brief Gère la normalisation du code de sortie (modulo 256)
 *
 * @param minishell Structure principale du shell
 * @param exit_nmb Code de sortie
 */
void	handle_exit_nmb(t_minishell *minishell, int exit_nmb)
{
	if (exit_nmb > 255 || exit_nmb < 0)
		ft_clean_exit(minishell, exit_nmb % 256);
	else
		ft_clean_exit(minishell, exit_nmb);
}

/**

	* @brief Affiche un message d'erreur formaté sur le descripteur de fichier spécifié
 *
 * @param prefix Préfixe du message
 * @param arg Argument contenant l'erreur
 * @param suffix Suffixe du message
 * @param fd Descripteur de fichier pour l'affichage
 */
void	ft_error_msg(char *prefix, char *arg, char *suffix, int fd)
{
	if (prefix)
		ft_putstr_fd(prefix, fd);
	if (arg)
		ft_putstr_fd(arg, fd);
	if (suffix)
		ft_putstr_fd(suffix, fd);
}

/**
 * @brief Vérifie si un caractère est un chiffre
 *
 * @param c Caractère à vérifier
 * @return int 1 si c'est un chiffre, 0 sinon
 */
static int	ft_isnum(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/**

	* @brief Vérifie si une chaîne ne contient que des chiffres et éventuellement un signe
 *
 * @param str Chaîne à vérifier
 * @return bool true si la chaîne est un nombre, false sinon
 */
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

/**
 * @brief Vérifie si une commande est un builtin
 *
 * @param value Nom de la commande
 * @return bool true si c'est un builtin, false sinon
 */
bool	ft_is_builtin(char *value)
{
	if (!value)
		return (false);
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

/**
 * @brief Affiche toutes les variables d'environnement
 *
 * @param env Liste des variables d'environnement
 */
void	ft_print_env(t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		ft_putendl_fd(current->var, 1);
		current = current->next;
	}
}

/**
 * @brief Convertit la liste des variables d'environnement en tableau de chaînes
 *
 * @param env Liste des variables d'environnement
 * @return char** Tableau de variables, NULL en cas d'erreur
 */
char	**ft_env_to_array(t_minishell *minishell, t_env *env)
{
	t_env	*current;
	char	**env_array;
	int		size;
	int		i;

	size = 0;
	current = env;
	while (current)
	{
		size++;
		current = current->next;
	}
	env_array = malloc(sizeof(char *) * (size + 1));
	if (!env_array)
		return (NULL);
	ft_gc_add(&minishell->gc_head, env_array);
	i = 0;
	current = env;
	while (current)
	{
		env_array[i] = ft_strdup(current->var);
		ft_gc_add(&minishell->gc_head, env_array[i]);
		if (!env_array[i])
		{
			while (i > 0)
			{
				ft_gc_remove(&minishell->gc_head, env_array[i]);
				free(env_array[--i]);
			}
			ft_gc_remove(&minishell->gc_head, env_array);
			free(env_array);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

/**
 * @brief Affiche les variables d'environnement au format export
 *
 * @param env Liste des variables d'environnement
 */
void	ft_print_export_list(t_env *env)
{
	t_env	*current;
	char	*equal_pos;

	current = env;
	while (current)
	{
		ft_putstr_fd("declare -x ", 1);
		equal_pos = ft_strchr(current->var, '=');
		if (equal_pos)
		{
			write(1, current->var, equal_pos - current->var + 1);
			ft_putchar_fd('"', 1);
			ft_putstr_fd(equal_pos + 1, 1);
			ft_putchar_fd('"', 1);
		}
		else
			ft_putstr_fd(current->var, 1);
		ft_putchar_fd('\n', 1);
		current = current->next;
	}
}

/**
 * @brief Gère la commande export pour une variable
 *
 * @param minishell Structure principale du shell
 * @param var Variable à exporter
 */
int	ft_handle_export_var(t_minishell *minishell, char **args)
{
	int		i;
	int		ret;
	char	*equal_pos;
	char	*var_name;
	t_env	*env_var;

	i = 1;
	ret = 0;
	while (args[i])
	{
		equal_pos = ft_strchr(args[i], '=');
		if (!ft_is_valid_identifier_before_equal(args[i]))
		{
			ft_putstr_fd("minishell: export: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			ret = 1;
		}
		else if (equal_pos)
		{
			var_name = ft_substr(args[i], 0, equal_pos - args[i]);
			env_var = ft_find_env_var(minishell->env, var_name);
			if (env_var)
			{
				ft_gc_remove(&minishell->gc_head, env_var->var);
				free(env_var->var);
				env_var->var = ft_strdup(args[i]);
			}
			else
				ft_add_env_var(minishell, args[i]);
			free(var_name);
		}
		else
		{
			if (!ft_find_env_var(minishell->env, args[i]))
				ft_add_env_var(minishell, args[i]);
		}
		i++;
	}
	return (ret);
}

/**
 * @brief Gère la commande unset pour une variable
 *
 * @param minishell Structure principale du shell
 * @param var_name Nom de la variable à supprimer
 */
int	ft_handle_unset_var(t_minishell *minishell, char **args)
{
	int		i;
	int		ret;
	t_env	*prev;
	t_env	*current;

	i = 1;
	ret = 0;
	while (args[i])
	{
		if (!ft_is_valid_identifier(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			ret = 1;
		}
		else
		{
			prev = NULL;
			current = minishell->env;
			while (current)
			{
				if (ft_env_var_match(current->var, args[i]))
				{
					if (prev)
						prev->next = current->next;
					else
						minishell->env = current->next;
					ft_gc_remove(&minishell->gc_head, current->var);
					free(current->var);
					ft_gc_remove(&minishell->gc_head, current);
					free(current);
					break ;
				}
				prev = current;
				current = current->next;
			}
		}
		i++;
	}
	return (ret);
}
