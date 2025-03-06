/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:36:19 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Initialise les données pour l'exécution de commandes
 *
 * @param cmd Structure de la première commande
 * @param cmd_count Pointeur pour stocker le nombre de commandes
 * @param minishell Structure principale du shell
 * @return pid_t* Tableau pour stocker les PIDs des processus enfants
 */
static pid_t *ft_init_cmd_execution(t_cmd *cmd, int *cmd_count, t_minishell *minishell)
{
	pid_t	*pids;
	t_cmd	*current;

	*cmd_count = 0;
	current = cmd;
	while (current)
	{
		(*cmd_count)++;
		current = current->next;
	}

	pids = (pid_t *)malloc(sizeof(pid_t) * (*cmd_count));
	if (!pids)
	{
		minishell->exit_nb = 1;
		return (NULL);
	}
	ft_gc_add(&minishell->gc_head, pids);

	// Fermer les descripteurs hérités problématiques
	close(3);
	close(4);

	return (pids);
}

/**
 * @brief Exécute une commande builtin
 *
 * @param current Commande à exécuter
 * @param minishell Structure principale du shell
 * @return int Code de retour de la commande
 */
static int ft_execute_builtin_cmd(t_cmd *current, t_minishell *minishell)
{
	int saved_stdin;
	int saved_stdout;
	int result;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	ft_setup_pipes(current);
	if (!ft_handle_redirection(current->redirs))
	{
		minishell->exit_nb = 1;
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	result = ft_execute_builtin(current, minishell);
	ft_restore_fds(saved_stdin, saved_stdout);
	ft_close_pipes(current);
	return (result);
}

/**
 * @brief Attend la fin des processus enfants
 *
 * @param pids Tableau des PIDs des processus enfants
 * @param cmd_count Nombre de commandes
 * @param minishell Structure principale du shell
 */
static void ft_wait_for_children(pid_t *pids, int cmd_count, t_minishell *minishell)
{
	int i;
	int status;

	for (i = 0; i < cmd_count; i++)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (WIFEXITED(status))
				minishell->exit_nb = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				minishell->exit_nb = 128 + WTERMSIG(status);
		}
	}
}

/**
 * @brief Prépare les pipes nécessaires pour une commande
 *
 * @param current Commande à préparer
 * @param minishell Structure principale du shell
 * @return int 1 si tout va bien, 0 si erreur
 */
static int ft_prepare_command_pipes(t_cmd *current, t_minishell *minishell)
{
	if (current->next != NULL)
	{
		if (!ft_create_pipe(current))
		{
			minishell->exit_nb = 1;
			return (0);
		}
		// Protéger les descripteurs du pipe
		ft_protect_fd(current->pipe_out);
		if (current->next)
			ft_protect_fd(current->next->pipe_in);
	}
	return (1);
}

/**
 * @brief Exécute une commande externe (non built-in)
 *
 * @param current Commande à exécuter
 * @param pids Tableau des PIDs
 * @param i Index dans le tableau des PIDs
 * @param minishell Structure principale du shell
 * @return int 1 si succès, 0 si erreur
 */
static int ft_execute_external_cmd(t_cmd *current, pid_t *pids, int i,
                                  t_minishell *minishell)
{
	pids[i] = fork();
	if (pids[i] == -1)
	{
		ft_putstr_fd("minishell: fork error\n", 2);
		minishell->exit_nb = 1;
		return (0);
	}

	if (pids[i] == 0)
	{
		ft_reset_signals();
		ft_execute_child(current, minishell);
	}
	else
	{
		ft_ignore_signals();
		ft_close_pipes(current);
	}

	return (1);
}

/**
 * @brief Exécute une commande, qu'elle soit built-in ou externe
 *
 * @param current Commande à exécuter
 * @param pids Tableau des PIDs
 * @param i Index dans le tableau des PIDs
 * @param minishell Structure principale du shell
 * @return int 1 si la commande a été exécutée, 0 si erreur
 */
static int ft_process_command(t_cmd *current, pid_t *pids, int i,
                            t_minishell *minishell)
{
	// Ignorer les commandes sans nom
	if (!current->name)
	{
		pids[i] = -1;
		return (1);
	}

	// Préparer les pipes
	if (!ft_prepare_command_pipes(current, minishell))
		return (0);

	// Gérer les commandes built-in
	if (ft_is_builtin(current->name))
	{
		minishell->exit_nb = ft_execute_builtin_cmd(current, minishell);
		pids[i] = -1;
		return (1);
	}

	// Pour les commandes non built-in, fork et exécuter
	return (ft_execute_external_cmd(current, pids, i, minishell));
}

/**
 * @brief Prépare et exécute une liste de commandes avec gestion des pipes
 *
 * @param cmd Structure de la commande à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_command(t_cmd *cmd, t_minishell *minishell)
{
	pid_t	*pids;
	int		cmd_count;
	t_cmd	*current;
	int		i;

	// Initialisation
	pids = ft_init_cmd_execution(cmd, &cmd_count, minishell);
	if (!pids)
		return;

	// Exécution de chaque commande
	current = cmd;
	i = 0;
	while (current && i < cmd_count)
	{
		if (ft_process_command(current, pids, i, minishell))
		{
			current = current->next;
			i++;
		}
		else
			break;
	}

	// Nettoyage et attente des enfants
	ft_wait_for_children(pids, cmd_count, minishell);
	ft_gc_remove(&minishell->gc_head, pids);
	free(pids);
}
