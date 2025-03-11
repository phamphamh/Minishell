/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_part2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:45:25 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Vérifie si une chaîne correspond à une commande interne
 *
 * @param value Chaîne à vérifier
 * @return true si commande interne, false sinon
 */
bool	ft_is_builtin(char *value)
{
	return (ft_strcmp(value, "echo") == 0 || ft_strcmp(value, "cd") == 0
		|| ft_strcmp(value, "pwd") == 0 || ft_strcmp(value, "export") == 0
		|| ft_strcmp(value, "unset") == 0 || ft_strcmp(value, "env") == 0
		|| ft_strcmp(value, "exit") == 0);
}

/**
 * @brief Affiche un message d'erreur pour unset
 *
 * @param var Nom de la variable
 * @param minishell Structure principale du shell
 */
void	ft_unset_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
}
