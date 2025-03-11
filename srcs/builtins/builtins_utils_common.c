/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_common.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:59:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:36:47 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Met à jour une variable d'environnement
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
	char	*new_var;
	char	*temp;

	current = env;
	while (current)
	{
		if (ft_env_var_match(current->var, name))
		{
			temp = ft_strjoin(name, "=");
			if (!temp)
				return ;
			new_var = ft_strjoin(temp, new_value);
			free(temp);
			if (!new_var)
				return ;
			free(current->var);
			current->var = new_var;
			ft_gc_add(&minishell->gc_head, new_var);
			return ;
		}
		current = current->next;
	}
}

/**
 * @brief Vérifie si une chaîne est composée uniquement de chiffres
 *
 * @param str Chaîne à vérifier
 * @return true si uniquement des chiffres, false sinon
 */
bool	ft_is_all_nb(char *str)
{
	int	i;

	if (!str)
		return (false);
	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i++;
	if (!str[i])
		return (false);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Affiche les variables d'environnement format env
 *
 * @param env Liste des variables d'environnement
 */
void	ft_print_env(t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strchr(current->var, '='))
		{
			ft_putstr_fd(current->var, 1);
			ft_putchar_fd('\n', 1);
		}
		current = current->next;
	}
}

/**
 * @brief Compte le nombre d'éléments dans la liste d'environnement
 *
 * @param env Liste des variables d'environnement
 * @return int Nombre d'éléments
 */
static int	ft_count_env_elements(t_env *env)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Remplit le tableau de variables d'environnement
 *
 * @param minishell Structure principale du shell
 * @param env Liste des variables d'environnement
 * @param env_array Tableau à remplir
 * @return char** Tableau rempli, NULL si erreur
 */
static char	**ft_fill_env_array(t_minishell *minishell, t_env *env,
				char **env_array)
{
	t_env	*current;
	int		i;

	i = 0;
	current = env;
	while (current)
	{
		env_array[i] = ft_strdup(current->var);
		if (!env_array[i])
		{
			while (--i >= 0)
			{
				ft_gc_remove(&minishell->gc_head, env_array[i]);
				free(env_array[i]);
			}
			ft_gc_remove(&minishell->gc_head, env_array);
			free(env_array);
			return (NULL);
		}
		ft_gc_add(&minishell->gc_head, env_array[i]);
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}
