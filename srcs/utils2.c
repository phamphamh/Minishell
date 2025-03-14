/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void	free_env_list(t_minishell *minishell)
{
	t_env	*current;
	t_env	*next;

	if (!minishell || !minishell->env)
		return ;
	// Éviter de libérer une liste déjà nettoyée
	if (minishell->env_cleaned)
		return;
	current = minishell->env;
	while (current)
	{
		next = current->next;
		if (current->var)
			free(current->var);
		free(current);
		current = next;
	}
	minishell->env = NULL;
	minishell->env_cleaned = true;
}

void	free_gc_list(t_minishell *minishell)
{
	t_gc_node	*current;
	t_gc_node	*next;

	if (!minishell->gc_head)
		return ;
	current = minishell->gc_head;
	while (current)
	{
		next = current->next;
		if (current->ptr)
			free(current->ptr);
		free(current);
		current = next;
	}
	minishell->gc_head = NULL;
}

// Fonction pour libérer les structures restantes qui ne seraient pas dans le GC
void	free_remaining_structures(t_minishell *minishell)
{
	(void)minishell; // Éviter le warning "unused parameter"
	// Nettoyage de l'historique Readline qui peut causer des "still reachable"
	rl_clear_history();
}

void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	int	fd;

	if (!minishell)
		exit(exit_num);

	// S'assurer que l'environnement n'est pas encore nettoyé
	minishell->env_cleaned = false;

	// Nettoyer d'abord les structures complexes
	if (minishell->tokens)
		ft_gc_remove_list(&minishell->gc_head, minishell->tokens);

	if (minishell->commands)
		ft_gc_remove_cmds(&minishell->gc_head, minishell->commands);

	// Nettoyer l'environnement
	ft_gc_remove_env(&minishell->gc_head, minishell->env);
	// Marquer que l'environnement a été nettoyé
	minishell->env_cleaned = true;
	free_env_list(minishell);  // Sera ignoré car env_cleaned est true

	// Libérer les autres structures qui pourraient ne pas être dans le GC
	free_remaining_structures(minishell);

	// Libérer tout ce qui reste dans le GC
	free_gc_list(minishell);

	// Assurons-nous de fermer tous les descripteurs de fichiers
	// y compris ceux qui pourraient avoir été dupliqués avec dup
	for (fd = 3; fd < 1024; fd++)
	{
		close(fd);
	}
	exit(exit_num);
}

/**
 * @brief Réinitialise les gestionnaires de signaux aux comportements par défaut
 */
void	ft_reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
