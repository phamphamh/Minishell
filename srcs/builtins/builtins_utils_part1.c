/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_part1.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:41:38 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Affiche un message d'erreur pour export
 *
 * @param var Nom de la variable
 * @param minishell Structure principale du shell
 */
void	ft_export_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
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
