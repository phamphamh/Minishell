/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 18:47:43 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:07:53 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Traite une commande unset pour une seule variable
 *
 * @param minishell Structure principale du shell
 * @param var_name Nom de la variable à supprimer
 */
static void	ft_process_unset_var(t_minishell *minishell, char *var_name)
{
	t_env	*current;
	t_env	*prev;
	t_env	*to_remove;

	if (!minishell || !var_name)
		return ;
	to_remove = ft_find_env_var(minishell->env, var_name);
	if (!to_remove)
		return ;
	current = minishell->env;
	prev = NULL;
	while (current)
	{
		if (current == to_remove)
		{
			if (prev)
				prev->next = current->next;
			else
				minishell->env = current->next;
			free(current->var);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

/**
 * @brief Traite une commande export pour une seule variable
 *
 * @param minishell Structure principale du shell
 * @param var Variable à exporter
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
static int	ft_process_export_var(t_minishell *minishell, char *var)
{
	t_env	*env_entry;
	char	*equal_pos;
	char	*var_name;
	int		name_len;

	equal_pos = ft_strchr(var, '=');
	if (!equal_pos)
	{
		// Si pas de signe égal, vérifier tout l'identifiant
		if (!ft_is_valid_identifier(var))
		{
			ft_export_error(var, minishell);
			return (1);
		}
		return (0);
	}

	name_len = equal_pos - var;
	var_name = ft_substr(var, 0, name_len);
	if (!var_name)
		return (1);

	if (!ft_is_valid_identifier(var_name))
	{
		ft_export_error(var_name, minishell);
		free(var_name);
		return (1);
	}

	env_entry = ft_find_env_var(minishell->env, var_name);
	if (env_entry)
	{
		free(env_entry->var);
		env_entry->var = ft_strdup(var);
	}
	else
		ft_add_env_var(minishell, var);
	free(var_name);
	return (0);
}

/**
 * @brief Traite une commande unset avec plusieurs arguments
 *
 * @param minishell Structure principale du shell
 * @param var_names Tableau de noms de variables à supprimer
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
int	ft_handle_unset_var(t_minishell *minishell, char **var_names)
{
	int	i;
	int	ret;

	if (!minishell || !var_names || !var_names[1])
		return (0);

	i = 1;
	ret = 0;
	while (var_names[i])
	{
		if (!ft_is_valid_identifier(var_names[i]))
		{
			ft_unset_error(var_names[i], minishell);
			ret = 1;
		}
		else
			ft_process_unset_var(minishell, var_names[i]);
		i++;
	}
	return (ret);
}

/**
 * @brief Traite une commande export avec plusieurs arguments
 *
 * @param minishell Structure principale du shell
 * @param vars Tableau de variables à exporter
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
int	ft_handle_export_var(t_minishell *minishell, char **vars)
{
	int	i;
	int	ret;

	if (!minishell || !vars || !vars[1])
		return (0);

	i = 1;
	ret = 0;
	while (vars[i])
	{
		if (ft_process_export_var(minishell, vars[i]))
			ret = 1;
		i++;
	}
	return (ret);
}

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
