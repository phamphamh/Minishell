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
int		g_signal_received = 0;

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
 * @brief Traite une ligne de commande entrée par l'utilisateur
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
	int		saved_stdin;
	int		saved_stdout;
	int		has_error;

	if (!line || !*line)
		return ;
	has_error = 0;
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
	if (cmd_count == 0)
		return ;
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return ;
	i = 0;
	while (i < cmd_count)
	{
		pids[i] = -1;
		i++;
	}
	ft_gc_add(&minishell->gc_head, pids);
	current = cmd;
	while (current && current->next && !has_error)
	{
		if (!ft_create_pipe(current))
		{
			minishell->exit_nb = 1;
			has_error = 1;
		}
		current = current->next;
	}
	if (!has_error)
	{
		i = 0;
		current = cmd;
		while (current && i < cmd_count && !has_error)
		{
			if (ft_is_builtin(current->name))
			{
				saved_stdin = dup(STDIN_FILENO);
				saved_stdout = dup(STDOUT_FILENO);
				if (saved_stdin == -1 || saved_stdout == -1)
				{
					ft_putstr_fd("minishell: dup error\n", 2);
					minishell->exit_nb = 1;
					has_error = 1;
				}
				else
				{
					ft_setup_pipes(current);
					if (!ft_handle_redirection(current, current->redirs))
					{
						minishell->exit_nb = 1;
						ft_restore_fds(saved_stdin, saved_stdout);
					}
					else
					{
						minishell->exit_nb = ft_execute_builtin(current,
								minishell);
						ft_restore_fds(saved_stdin, saved_stdout);
					}
					ft_close_pipes(current);
				}
			}
			else
			{
				pids[i] = fork();
				if (pids[i] == -1)
				{
					ft_putstr_fd("minishell: fork error\n", 2);
					minishell->exit_nb = 1;
					has_error = 1;
				}
				else if (pids[i] == 0)
				{
					ft_reset_signals();
					ft_setup_pipes(current);
					ft_close_unused_fds(current);
					if (!ft_handle_redirection(current, current->redirs))
					{
						ft_clean_exit(minishell, 1);
					}
					ft_execute_child(current, minishell);
					exit(EXIT_FAILURE);
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
	}
	ft_close_all_pipes(cmd);
	i = 0;
	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)
			{
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
	ft_clean_exit(&minishell, minishell.exit_nb);
	return (0);
}
