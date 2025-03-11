/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/25 12:00:24 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 15:01:34 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* REMARQUE: La fonction ft_env_var_match a été déplacée vers utils.c
   pour éviter les redéfinitions et les incompatibilités de type */

/* REMARQUE: La fonction ft_find_env_var a été déplacée vers utils.c
   pour éviter les redéfinitions et les fuites mémoire */

/* Compte le nombre de variables dans l'environnement */
int	ft_env_list_size(t_env *env)
{
	int		size;
	t_env	*current;

	size = 0;
	current = env;
	while (current)
	{
		size++;
		current = current->next;
	}
	return (size);
}

/* Trie le tableau des variables d'environnement par ordre alphabétique */
void	ft_bubble_sort_env(t_env **arr, int size)
{
	int		i;
	int		j;
	t_env	*temp;

	i = 0;
	while (i < size - 1)
	{
		j = 0;
		while (j < size - i - 1)
		{
			if (ft_strcmp(arr[j]->var, arr[j + 1]->var) > 0)
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

/* Crée un tableau trié des variables d'environnement */
t_env	**ft_sort_env_list(t_env *env, int size)
{
	t_env	**arr;
	int		i;
	t_env	*current;

	arr = malloc(sizeof(t_env *) * size);
	if (!arr)
		return (NULL);
	i = 0;
	current = env;
	while (current)
	{
		arr[i++] = current;
		current = current->next;
	}
	ft_bubble_sort_env(arr, size);
	return (arr);
}

/* Affiche une variable d'environnement au format export */
void	ft_print_export_var(t_env *env_var)
{
	char	*equal_pos;

	ft_putstr_fd("declare -x ", 1);
	equal_pos = ft_strchr(env_var->var, '=');
	if (equal_pos)
	{
		write(1, env_var->var, equal_pos - env_var->var + 1);
		ft_putchar_fd('"', 1);
		ft_putstr_fd(equal_pos + 1, 1);
		ft_putchar_fd('"', 1);
	}
	else
		ft_putstr_fd(env_var->var, 1);
	ft_putchar_fd('\n', 1);
}

/* Affiche la liste des variables d'environnement au format export */
void	ft_print_export_list(t_env *env)
{
	t_env	**sorted_env;
	int		i;
	int		env_size;

	env_size = ft_env_list_size(env);
	sorted_env = ft_sort_env_list(env, env_size);
	if (!sorted_env)
		return ;
	i = 0;
	while (i < env_size)
	{
		ft_print_export_var(sorted_env[i]);
		i++;
	}
	free(sorted_env);
}

/* Supprime une variable spécifique de l'environnement */
void	ft_handle_unset_var(t_minishell *minishell, char *var_name)
{
	t_env	*current;
	t_env	*prev;
	t_env	*to_remove;

	if (!minishell || !var_name)
		return;

	to_remove = ft_find_env_var(minishell->env, var_name);
	if (!to_remove)
		return;

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
			return;
		}
		prev = current;
		current = current->next;
	}
}

/* Ajoute ou met à jour une variable d'environnement */
void	ft_handle_export_var(t_minishell *minishell, char *var)
{
	t_env	*env_entry;
	char	*equal_pos;
	char	*var_name;
	int		name_len;

	equal_pos = ft_strchr(var, '=');
	if (!equal_pos)
		return;
	name_len = equal_pos - var;
	var_name = ft_substr(var, 0, name_len);
	if (!var_name)
		return;
	if (!ft_is_valid_identifier(var_name))
	{
		ft_export_error(var_name, minishell);
		free(var_name);
		return;
	}
	env_entry = ft_find_env_var(minishell->env, var_name);
	if (env_entry)
	{
		ft_gc_remove(&minishell->gc_head, env_entry->var);
		env_entry->var = ft_strdup(var);
		if (env_entry->var)
			ft_gc_add(&minishell->gc_head, env_entry->var);
	}
	else
		ft_add_env_var(minishell, var);
	free(var_name);
}

/* Affiche un message d'erreur pour export */
void	ft_export_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
}

/* Affiche un message d'erreur pour unset */
void	ft_unset_error(char *var, t_minishell *minishell)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	minishell->exit_nb = 1;
}

/* REMARQUE: Les fonctions ft_is_valid_identifier et ft_is_valid_identifier_before_equal
   ont été déplacées vers utils.c pour éviter les redéfinitions */
