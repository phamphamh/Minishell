/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 14:05:01 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/**
 * @brief Initialise l'environnement et le garbage collector
 *
 * @param minishell Structure principale du shell
 * @param envp Tableau des variables d'environnement
 */
static void	set_default_values(t_minishell *minishell)
{
	minishell->gc_head = NULL;
	minishell->env = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
	minishell->env_cleaned = false;
}

static bool	has_path_variable(t_env *env)
{
	while (env)
	{
		if (ft_strncmp(env->var, "PATH=", 5) == 0)
			return (true);
		env = env->next;
	}
	return (false);
}

void	ft_initialize(t_minishell *minishell, char **envp)
{
	t_env	*env_current;

	set_default_values(minishell);
	minishell->env = ft_env_to_list(envp, minishell);
	check_env_initialization(minishell, envp);
	env_current = minishell->env;
	while (env_current)
	{
		ft_gc_add(&minishell->gc_head, env_current);
		if (env_current->var)
			ft_gc_add(&minishell->gc_head, env_current->var);
		env_current = env_current->next;
	}
	if (!has_path_variable(minishell->env))
		set_default_path(minishell);
}

/**
 * @brief Initialise la structure principale du shell
 *
 * @param minishell Structure à initialiser
 */
void	ft_initialize_shell(t_minishell *minishell)
{
	if (minishell)
	{
		minishell->tokens = NULL;
		minishell->commands = NULL;
	}
}

/**
 * @brief Point d'entrée du programme
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau des arguments
 * @param envp Tableau des variables d'environnement
 * @return int Code de sortie
 */
int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;
	char		*line;

	if (argc > 1)
	{
		ft_putstr_fd("Usage: ./minishell\n", 2);
		return (1);
	}
	(void)argv;
	ft_initialize(&minishell, envp);
	ft_setup_signals();
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
		{
			ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (*line)
			add_history(line);
		ft_process_line(line, &minishell);
		free(line);
	}
	return (ft_clean_exit(&minishell, minishell.exit_nb), minishell.exit_nb);
}
