/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/25 12:00:24 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/08 20:53:31 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* Vérifie si une variable correspond à un nom donné */
int	ft_env_var_match(const char *env_var, const char *var_name)
{
	size_t	name_len;
	char	*equal_pos;

	name_len = ft_strlen(var_name);
	equal_pos = ft_strchr(env_var, '=');
	if (equal_pos)
		return (ft_strncmp(env_var, var_name, name_len) == 0
			&& env_var[name_len] == '=');
	return (ft_strcmp(env_var, var_name) == 0);
}

/* Recherche une variable dans l'environnement */
t_env	*ft_find_env_var(t_env *env, const char *var)
{
	t_env	*current;
	size_t	name_len;
	char	*equal_pos;

	equal_pos = ft_strchr(var, '=');
	name_len = equal_pos ? (size_t)(equal_pos - var) : ft_strlen(var);
	current = env;
	while (current)
	{
		if (ft_strncmp(current->var, var, name_len) == 0
			&& (current->var[name_len] == '=' || current->var[name_len] == '\0'))
			return (current);
		current = current->next;
	}
	return (NULL);
}

/* Ajoute une nouvelle variable à l'environnement */
void	ft_add_env_var(t_minishell *minishell, const char *var)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return ;
	new_node->var = ft_strdup(var);
	if (!new_node->var)
	{
		free(new_node);
		return ;
	}
	new_node->next = minishell->env;
	minishell->env = new_node;
}

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

	current = minishell->env;
	prev = NULL;
	while (current)
	{
		if (ft_env_var_match(current->var, var_name))
		{
			if (prev)
				prev->next = current->next;
			else
				minishell->env = current->next;
			ft_gc_remove(&minishell->gc_head, current->var);
			ft_gc_remove(&minishell->gc_head, current);
			return ;
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

/* Vérifie si un identifiant est valide selon les règles POSIX */
int	ft_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str || ft_isdigit(*str))
		return (0);
	i = 0;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/* Vérifie si la partie avant le '=' d'une variable est un identifiant valide */
int	ft_is_valid_identifier_before_equal(const char *str)
{
	char	*equal_pos;
	char	*name;
	int		result;

	if (!str || !*str)
		return (0);
	equal_pos = ft_strchr(str, '=');
	if (!equal_pos)
		return (ft_is_valid_identifier(str));
	name = ft_substr(str, 0, equal_pos - str);
	if (!name)
		return (0);
	result = ft_is_valid_identifier(name);
	free(name);
	return (result);
}
