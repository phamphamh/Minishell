/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 15:09:56 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void	check_env_initialization(t_minishell *minishell, char **envp)
{
	if (!minishell->env && envp)
	{
		ft_putstr_fd("Erreur: Impossible d'initialiser l'environnement\n", 2);
		ft_clean_exit(minishell, 1);
	}
}

/**
 * @brief Nettoie la liste des variables d'environnement
 *
 * @param env Pointeur vers la liste des variables d'environnement
 */
void	ft_clean_env_list(t_env *env)
{
	t_env	*current;
	t_env	*next;

	current = env;
	while (current)
	{
		next = current->next;
		if (current->var)
			free(current->var);
		free(current);
		current = next;
	}
}

static t_env	*ft_create_env_node(char *var, t_env *env)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
	{
		free(var);
		ft_clean_env_list(env);
		return (NULL);
	}
	new->var = var;
	new->next = NULL;
	return (new);
}

static void	ft_add_env_node(t_env **env, t_env **last, t_env *new)
{
	if (!(*env))
		*env = new;
	else
		(*last)->next = new;
	*last = new;
}

t_env	*ft_env_to_list(char **envp, t_minishell *minishell)
{
	t_env	*env;
	t_env	*new;
	t_env	*last;
	int		i;
	char	*var;

	(void)minishell;
	env = NULL;
	last = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		var = ft_strdup(envp[i]);
		if (!var)
			return (ft_clean_env_list(env), NULL);
		new = ft_create_env_node(var, env);
		if (!new)
			return (NULL);
		ft_add_env_node(&env, &last, new);
		i++;
	}
	return (env);
}
