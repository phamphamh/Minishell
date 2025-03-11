/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 13:59:09 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 17:32:38 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

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
 * @brief Vérifie si une chaîne contient uniquement des chiffres
 *
 * @param str Chaîne à vérifier
 * @return true si la chaîne ne contient que des chiffres, false sinon
 */
bool	ft_is_all_nb(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isnum(str[i]))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Vérifie si la commande est un builtin
 *
 * @param value Nom de la commande
 * @return true si c'est un builtin, false sinon
 */
bool	ft_is_builtin(char *value)
{
	return (ft_strcmp(value, "echo") == 0
		|| ft_strcmp(value, "cd") == 0
		|| ft_strcmp(value, "pwd") == 0
		|| ft_strcmp(value, "export") == 0
		|| ft_strcmp(value, "unset") == 0
		|| ft_strcmp(value, "env") == 0
		|| ft_strcmp(value, "exit") == 0);
}

/**
 * @brief Affiche les variables d'environnement au format de la commande env
 *
 * @param env Liste des variables d'environnement
 */
void	ft_print_env(t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		// Afficher uniquement les variables avec une valeur (contenant un '=')
		if (ft_strchr(current->var, '='))
		{
			ft_putstr_fd(current->var, 1);
			ft_putchar_fd('\n', 1);
		}
		current = current->next;
	}
}

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
 * @brief Affiche toutes les variables d'environnement au format d'export
 *
 * @param env Liste des variables d'environnement
 */
void	ft_print_export_list(t_env *env)
{
	t_env	*current;
	t_env	temp;

	current = env;
	while (current)
	{
		// Créer un t_env temporaire pour l'affichage
		temp.var = current->var;
		temp.next = NULL;

		ft_print_export_var(&temp);

		current = current->next;
	}
}

/**
 * @brief Traite l'unset d'une variable spécifique
 *
 * @param minishell Structure principale du shell
 * @param var_name Nom de la variable à supprimer
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
static int	ft_process_unset_var(t_minishell *minishell, char *var_name)
{
	if (!ft_is_valid_identifier(var_name)) // Vérification du nom valide
	{
		ft_putstr_fd("minishell: unset: `", 2);
		ft_putstr_fd(var_name, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	else
	{
		t_env *prev = NULL;
		t_env *current = minishell->env;

		while (current)
		{
			if (ft_env_var_match(current->var, var_name)) // Trouve la variable à supprimer
			{
				if (prev)
					prev->next = current->next;
				else
					minishell->env = current->next;

				ft_gc_remove(&minishell->gc_head, current->var);
				free(current->var);
				ft_gc_remove(&minishell->gc_head, current);
				free(current);
				break;
			}
			prev = current;
			current = current->next;
		}
	}
	return (0);
}

/**
 * @brief Gère la commande unset pour une variable
 *
 * @param minishell Structure principale du shell
 * @param args Arguments de la commande
 * @return int Code de retour (0 en cas de succès, 1 en cas d'erreur)
 */
int	ft_handle_unset_cmd(t_minishell *minishell, char **args)
{
	int i = 1;
	int ret = 0;
	int current_ret;

	while (args[i])
	{
		current_ret = ft_process_unset_var(minishell, args[i]);
		if (current_ret)
			ret = 1;
		i++;
	}
	return (ret);
}

