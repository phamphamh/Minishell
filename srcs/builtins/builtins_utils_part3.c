/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_part3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 12:13:32 by yboumanz         ###   ########.fr       */
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
	return (ft_strcmp_trim(value, "echo") == 0 || ft_strcmp_trim(value,
			"cd") == 0 || ft_strcmp_trim(value, "pwd") == 0
		|| ft_strcmp_trim(value, "export") == 0 || ft_strcmp_trim(value,
			"unset") == 0 || ft_strcmp_trim(value, "env") == 0
		|| ft_strcmp_trim(value, "exit") == 0);
}

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
			ft_gc_remove(&minishell->gc_head, current->var);
			free(current->var);
			current->var = new_var;
			ft_gc_add(&minishell->gc_head, new_var);
			return ;
		}
		current = current->next;
	}
}

void	ft_remove_env_var(t_minishell *minishell, t_env *to_remove)
{
	t_env	*current;
	t_env	*prev;

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

int	ft_validate_export_var(t_minishell *minishell, char *var, char **var_name,
		char **equal_pos)
{
	*equal_pos = ft_strchr(var, '=');
	if (!*equal_pos)
	{
		*var_name = NULL;
		if (!ft_is_valid_identifier(var))
		{
			ft_export_error(var, minishell);
			return (1);
		}
		*var_name = ft_strdup(var);
		if (!*var_name)
			return (1);
		return (0);
	}
	*var_name = ft_substr(var, 0, *equal_pos - var);
	if (!*var_name)
		return (1);
	if (!ft_is_valid_identifier(*var_name))
	{
		ft_export_error(*var_name, minishell);
		free(*var_name);
		return (1);
	}
	return (0);
}
