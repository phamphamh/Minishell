/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:29:32 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

// Initialisation de la variable globale pour les signaux
int	g_signal_received = 0;

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

	env = NULL;
	last = NULL;
	i = 0;
	while (envp[i])
	{
		var = ft_strdup(envp[i]);
		if (!var)
			return (NULL);
		if (!ft_gc_add(&minishell->gc_head, var))
		{
			free(var);
			return (NULL);
		}
		new = malloc(sizeof(t_env));
		if (!new)
			return (NULL);
		if (!ft_gc_add(&minishell->gc_head, new))
		{
			free(new);
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
 * @brief Initialise l'environnement et le garbage collector
 *
 * @param minishell Structure principale du shell
 * @param envp Tableau des variables d'environnement
 */
void	ft_initialize(t_minishell *minishell, char **envp)
{
	minishell->env = ft_env_to_list(envp, minishell);
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
}

/**
 * @brief Traite une ligne de commande
 *
 * @param line Ligne de commande à traiter
 * @param minishell Structure principale du shell
 */
void	ft_process_line(char *line, t_minishell *minishell)
{
	t_token	*tokens;
	t_cmd	*cmd;
	t_cmd	*current;
	pid_t	*pids;
	int		cmd_count;
	int		i;
	int		status;

	if (!line || !*line)
		return ;
	add_history(line);
	minishell->tokens = NULL;
	minishell->commands = NULL;
	tokens = ft_tokenize(line, minishell);
	if (!tokens)
		return ;
	minishell->tokens = tokens;
	if (ft_check_syntax_errors(tokens))
		return ;
	cmd = tokens_to_cmds(tokens, minishell);
	if (!cmd)
		return ;
	minishell->commands = cmd;
	cmd_count = 0;
	current = cmd;
	while (current)
	{
		cmd_count++;
		current = current->next;
	}
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return ;
	ft_gc_add(&minishell->gc_head, pids);
	for (i = 0; i < cmd_count; i++)
		pids[i] = -1;
	i = 0;
	current = cmd;
	while (current)
	{
		if (current->next && !ft_create_pipe(current))
		{
			minishell->exit_nb = 1;
			break ;
		}
		if (ft_is_builtin(current->name))
		{
			int saved_stdin = dup(STDIN_FILENO);
			int saved_stdout = dup(STDOUT_FILENO);

			if (saved_stdin == -1 || saved_stdout == -1)
			{
				if (saved_stdin != -1)
					close(saved_stdin);
				if (saved_stdout != -1)
					close(saved_stdout);
				minishell->exit_nb = 1;
				break;
			}

			ft_setup_pipes(current);
			if (!ft_handle_redirection(current->redirs))
			{
				minishell->exit_nb = 1;
				// ft_restore_fds est déjà appelé dans ft_handle_redirection en cas d'erreur
			}
			else
			{
				minishell->exit_nb = ft_execute_builtin(current, minishell);
				ft_restore_fds(saved_stdin, saved_stdout);
			}
			ft_close_pipes(current);
		}
		else
		{
			pids[i] = fork();
			if (pids[i] == -1)
			{
				ft_putstr_fd("minishell: fork error\n", 2);
				minishell->exit_nb = 1;
				break ;
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
		}
		current = current->next;
		i++;
	}

	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (WIFEXITED(status))
				minishell->exit_nb = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				minishell->exit_nb = 128 + WTERMSIG(status);
				if (WTERMSIG(status) == SIGINT)
					ft_putstr_fd("\n", 1);
				else if (WTERMSIG(status) == SIGQUIT)
					ft_putstr_fd("Quit (core dumped)\n", 1);
			}
		}
		i++;
	}
	ft_setup_signals();
	ft_gc_remove(&minishell->gc_head, pids);
	free(pids);
}

/**
 * @brief Initialise la structure principale du shell
 *
 * @param minishell Structure à initialiser
 */
void	ft_initialize_shell(t_minishell *minishell)
{
	minishell->env = NULL;
	minishell->gc_head = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
}

/**
 * @brief Point d'entrée principal du programme
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @param envp Tableau des variables d'environnement
 * @return int Code de retour
 */
int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;
	char		*line;
	int			saved_stdin;
	int			saved_stdout;
	int			exceptions[3];

	(void)argc;
	(void)argv;
	ft_initialize_shell(&minishell);
	ft_initialize(&minishell, envp);
	ft_setup_signals();

	// Sauvegarder les descripteurs de fichiers standards au début
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);

	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (g_signal_received)
		{
			free(line);
			ft_setup_signals();
			continue ;
		}
		if (*line)
		{
			ft_process_line(line, &minishell);
		}
		free(line);

		// Fermer tous les descripteurs non-standards après chaque commande
		exceptions[0] = saved_stdin;
		exceptions[1] = saved_stdout;
		exceptions[2] = -1;
		ft_close_all_fds(exceptions);
		ft_setup_signals();
	}

	// Restaurer les descripteurs standards avant de quitter
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);

	ft_clean_exit(&minishell, minishell.exit_nb);
	return (0);
}
