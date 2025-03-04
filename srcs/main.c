/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/04 21:01:39 by tcousin          ###   ########.fr       */
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

			ft_setup_pipes(current);
			if (!ft_handle_redirection(current->redirs))
			{
				minishell->exit_nb = 1;
				ft_restore_fds(saved_stdin, saved_stdout);
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

	(void)argc;
	(void)argv;
	ft_initialize_shell(&minishell);
	ft_initialize(&minishell, envp);
	ft_setup_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			ft_clean_exit(&minishell, minishell.exit_nb);
		}
		if (g_signal_received)
		{
			free(line);
			ft_setup_signals();
			continue ;
		}
		ft_process_line(line, &minishell);
		free(line);
		ft_setup_signals();
	}
	return (0);
}
