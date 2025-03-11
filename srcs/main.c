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

// Initialisation de la variable globale pour les signaux
int			g_signal_received = 0;

/**
 * @brief Transforme les variables d'environnement en liste chaînée
 *
 * @param envp Tableau des variables d'environnement
 * @param minishell Structure principale du shell
 * @return t_env* Liste chaînée initialisée, NULL en cas d'erreur
 */
t_env	*ft_env_to_list(char **envp, t_minishell *minishell)
{
	t_env	*env;
	t_env	*new;
	t_env	*last;
	char	*var;
	int		i;

	(void)minishell;
	env = NULL;
	last = NULL;
	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i])
	{
		var = ft_strdup(envp[i]);
		if (!var)
		{
			ft_clean_env_list(env);
			return (NULL);
		}
		new = malloc(sizeof(t_env));
		if (!new)
		{
			free(var);
			ft_clean_env_list(env);
			return (NULL);
		}
		new->var = var;
		new->next = NULL;
		if (!env)
			env = new;
		else
			last->next = new;
		last = new;
		i++;
	}
	return (env);
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

/**
 * @brief Initialise l'environnement et le garbage collector
 *
 * @param minishell Structure principale du shell
 * @param envp Tableau des variables d'environnement
 */
void	ft_initialize(t_minishell *minishell, char **envp)
{
	bool	has_path;
	t_env	*env_node;
	char	*default_path;

	minishell->gc_head = NULL;
	minishell->env = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
	minishell->env = ft_env_to_list(envp, minishell);
	if (!minishell->env && envp)
	{
		ft_putstr_fd("Erreur: Impossible d'initialiser l'environnement\n", 2);
		ft_clean_exit(minishell, 1);
	}
	has_path = false;
	env_node = minishell->env;
	while (env_node)
	{
		if (ft_strncmp(env_node->var, "PATH=", 5) == 0)
		{
			has_path = true;
			break ;
		}
		env_node = env_node->next;
	}
	if (!has_path)
	{
		default_path = ft_strdup("PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:.");
		if (default_path)
		{
			ft_add_env_var(minishell, default_path);
			free(default_path);
		}
	}
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
 * @brief Initialise le shell et vérifie l'environnement
 *
 * @param minishell Structure principale du shell
 * @param envp Tableau des variables d'environnement
 */
static void	init_minishell(t_minishell *minishell, char **envp)
{
	ft_initialize(minishell, envp);
	ft_setup_signals();
}

/**
 * @brief Boucle principale du shell, récupérant et exécutant les commandes
 *
 * @param minishell Structure principale du shell
 */
static void	shell_loop(t_minishell *minishell)
{
	char	*line;

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
		ft_process_line(line, minishell);
		free(line);
	}
}

/**
 * @brief Point d'entrée du programme principal
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau des arguments
 * @param envp Tableau des variables d'environnement
 * @return int Code de sortie
 */
int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	if (argc > 1)
	{
		ft_putstr_fd("Usage: ./minishell\n", 2);
		return (1);
	}
	(void)argv;
	init_minishell(&minishell, envp);
	shell_loop(&minishell);
	ft_clean_exit(&minishell, minishell.exit_nb);
	return (0);
}
