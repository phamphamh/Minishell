/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_common.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:59:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:59:59 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

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

/**
 * @brief Convertit la liste d'environnement en tableau
 *
 * @param minishell Structure principale du shell
 * @param env Liste des variables d'environnement
 * @return char** Tableau de variables d'environnement, NULL si erreur
 */
char	**ft_env_to_array(t_minishell *minishell, t_env *env)
{
	char	**env_array;
	int		count;

	count = ft_count_env_elements(env);
	env_array = (char **)malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	ft_gc_add(&minishell->gc_head, env_array);
	return (ft_fill_env_array(minishell, env, env_array));
}
