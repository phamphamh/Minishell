/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 17:27:42 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

// Initialisation de la variable globale pour les signaux
int		g_signal_received = 0;

/**
 * @brief Crée un nouveau noeud pour la liste d'environnement
 *
 * @param var_str Chaîne de la variable d'environnement
 * @return t_env* Nouveau noeud créé, NULL en cas d'erreur
 */
t_env	*ft_create_env_node(char *var_str)
{
	t_env	*new;
	char	*var;

	var = ft_strdup(var_str);
	if (!var)
		return (NULL);
	new = malloc(sizeof(t_env));
	if (!new)
	{
		free(var);
		return (NULL);
	}
	new->var = var;
	new->next = NULL;
	return (new);
}

/**
 * @brief Traite un élément de l'environnement et l'ajoute à la liste
 *
 * @param env Pointeur vers la liste des variables d'environnement
 * @param last Pointeur vers le dernier élément de la liste
 * @param env_var Variable d'environnement à ajouter
 * @return t_env* Nouveau dernier élément, NULL en cas d'erreur
 */
static t_env	*ft_process_env_var(t_env **env, t_env *last, char *env_var)
{
	t_env	*new;

	new = ft_create_env_node(env_var);
	if (!new)
		return (NULL);

	if (!*env)
		*env = new;
	else
		last->next = new;

	return (new);
}

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
	t_env	*last;
	int		i;

	(void)minishell;
	env = NULL;
	last = NULL;
	i = 0;
	if (!envp)
		return (NULL);

	while (envp[i])
	{
		last = ft_process_env_var(&env, last, envp[i]);
		if (!last)
		{
			ft_clean_env_list(env);
			return (NULL);
		}
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
 * @brief Vérifie si PATH existe dans l'environnement
 *
 * @param env Liste des variables d'environnement
 * @return bool true si PATH existe, false sinon
 */
static bool	ft_has_path_in_env(t_env *env)
{
	t_env	*env_node;

	env_node = env;
	while (env_node)
	{
		if (ft_strncmp(env_node->var, "PATH=", 5) == 0)
			return (true);
		env_node = env_node->next;
	}
	return (false);
}

/**
 * @brief Vérifie si PATH existe dans l'environnement et l'ajoute si nécessaire
 *
 * @param minishell Structure principale du shell
 */
void	ft_check_and_add_path(t_minishell *minishell)
{
	char	*default_path;

	if (!ft_has_path_in_env(minishell->env))
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
 * @brief Initialise l'environnement et le garbage collector
 *
 * @param minishell Structure principale du shell
 * @param envp Tableau des variables d'environnement
 */
void	ft_initialize(t_minishell *minishell, char **envp)
{
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

	ft_check_and_add_path(minishell);
}

/**
 * @brief Affiche la liste des tokens générés
 *
 * @param tokens Liste des tokens à afficher
 */
void	ft_print_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	printf("🔹 Liste des tokens générés :\n");
	while (current)
	{
		printf("Token: \"%s\" | Type: %d\n", current->value, current->type);
		current = current->next;
	}
	printf("🔹 Fin de la liste des tokens\n");
}

/**
 * @brief Affiche les redirections et les pipes d'une commande
 *
 * @param current Commande dont il faut afficher les redirections et pipes
 */
void	ft_print_cmd_redirections(t_cmd *current)
{
	t_redirection	*redir;

	printf("   - Redirections:\n");
	redir = current->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
			printf("     ⏩ Input  (<) -> %s\n", redir->file);
		else if (redir->type == TOKEN_REDIR_OUT)
			printf("     ⏩ Output (>) -> %s\n", redir->file);
		else if (redir->type == TOKEN_REDIR_APPEND)
			printf("     ⏩ Append (>>) -> %s\n", redir->file);
		else if (redir->type == TOKEN_HEREDOC)
			printf("     ⏩ Here-Doc (<<) -> %s\n", redir->file);
		redir = redir->next;
	}
	printf("   - Pipe_in: %d, Pipe_out: %d\n", current->pipe_in,
		current->pipe_out);
}

/**
 * @brief Affiche les arguments d'une commande
 *
 * @param args Tableau des arguments
 */
static void	ft_print_cmd_args(char **args)
{
	int	arg_index;

	printf("   - Arguments: ");
	if (args)
	{
		arg_index = 0;
		while (args[arg_index])
		{
			printf("\"%s\" ", args[arg_index]);
			arg_index++;
		}
		printf("\n");
	}
	else
		printf("(null)\n");
}

// DEBUG DELETE AFTER
void	ft_print_commands(t_cmd *cmds)
{
	t_cmd			*current;
	int				cmd_index;

	current = cmds;
	cmd_index = 0;
	printf("\n🔹 Liste des commandes générées :\n");
	while (current)
	{
		printf("🔹 Commande %d:\n", cmd_index);
		printf("   - Nom: %s\n", current->name ? current->name : "(null)");
		ft_print_cmd_args(current->args);
		ft_print_cmd_redirections(current);
		current = current->next;
		cmd_index++;
	}
	printf("🔹 Fin de la liste des commandes\n");
}

/**
 * @brief Compte le nombre de commandes dans la liste
 *
 * @param cmd Liste des commandes
 * @return int Nombre de commandes
 */
static int	ft_count_commands(t_cmd *cmd)
{
	int		cmd_count;
	t_cmd	*current;

	cmd_count = 0;
	current = cmd;
	while (current)
	{
		cmd_count++;
		current = current->next;
	}
	return (cmd_count);
}

/**
 * @brief Alloue et initialise un tableau de PIDs pour les processus enfants
 *
 * @param minishell Structure principale du shell
 * @param cmd_count Nombre de commandes
 * @return pid_t* Tableau de PIDs alloué, NULL en cas d'erreur
 */
static pid_t	*ft_init_pid_array(t_minishell *minishell, int cmd_count)
{
	pid_t	*pids;

	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (NULL);
	ft_gc_add(&minishell->gc_head, pids);
	return (pids);
}

/**
 * @brief Gère le processus enfant pour exécuter une commande externe
 *
 * @param current Commande à exécuter
 * @param minishell Structure principale du shell
 */
static void	ft_handle_child_exec(t_cmd *current, t_minishell *minishell)
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

/**
 * @brief Exécute une commande spécifique
 *
 * @param current Commande à exécuter
 * @param minishell Structure principale du shell
 * @param cmd_count Nombre total de commandes
 * @param i Index de la commande
 * @param pids Tableau des PIDs
 * @return int 1 si erreur, 0 sinon
 */
static int	ft_execute_single_command(t_cmd *current, t_minishell *minishell,
									int cmd_count, int i, pid_t *pids)
{
	if (ft_is_builtin(current->name) && cmd_count == 1)
	{
		minishell->exit_nb = ft_execute_builtin(current, minishell);
		ft_restore_fds(0, 1);
		return (0);
	}
	else
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			ft_putstr_fd("minishell: fork error\n", 2);
			minishell->exit_nb = 1;
			return (1);
		}
		else if (pids[i] == 0)
			ft_handle_child_exec(current, minishell);
		else
		{
			ft_ignore_signals();
			ft_close_pipes(current);
		}
		return (0);
	}
}

/**
 * @brief Met à jour le code de sortie en fonction du statut d'un processus
 *
 * @param status Statut du processus
 * @param minishell Structure principale du shell
 */
static void	ft_update_exit_status(int status, t_minishell *minishell)
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

/**
 * @brief Attend la fin des processus enfants et met à jour le code de sortie
 *
 * @param pids Tableau des PIDs
 * @param cmd_count Nombre de commandes
 * @param minishell Structure principale du shell
 */
static void	ft_wait_for_commands(pid_t *pids, int cmd_count, t_minishell *minishell)
{
	int	i;
	int	status;

	i = -1;
	while (++i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)
				ft_update_exit_status(status, minishell);
		}
	}
	ft_setup_signals();
}

/**
 * @brief Exécute toutes les commandes dans la liste
 *
 * @param cmd Première commande
 * @param minishell Structure principale du shell
 * @param pids Tableau des PIDs
 * @param cmd_count Nombre total de commandes
 * @return int 1 si erreur, 0 sinon
 */
static int	ft_execute_all_commands(t_cmd *cmd, t_minishell *minishell,
								pid_t *pids, int cmd_count)
{
	t_cmd	*current;
	int		i;
	int		has_error;

	current = cmd;
	i = 0;
	has_error = 0;
	while (current && !has_error)
	{
		if (ft_handle_redirection(current, current->redirs))
		{
			has_error = ft_execute_single_command(current, minishell, cmd_count, i, pids);
			current = current->next;
			i++;
		}
	}
	return (has_error);
}

/**
 * @brief Exécute les commandes après leur création
 *
 * @param cmd Liste des commandes à exécuter
 * @param minishell Structure principale du shell
 */
void	ft_execute_commands(t_cmd *cmd, t_minishell *minishell)
{
	int		cmd_count;
	pid_t	*pids;

	cmd_count = ft_count_commands(cmd);
	pids = ft_init_pid_array(minishell, cmd_count);
	if (!pids)
		return ;

	ft_execute_all_commands(cmd, minishell, pids, cmd_count);

	ft_close_all_pipes(cmd);
	ft_wait_for_commands(pids, cmd_count, minishell);
	ft_gc_remove(&minishell->gc_head, pids);
	free(pids);
}

/**
 * @brief Vérifie la syntaxe et crée les commandes à partir des tokens
 *
 * @param tokens Tokens à traiter
 * @param minishell Structure principale du shell
 * @return t_cmd* Commandes créées, NULL en cas d'erreur
 */
static t_cmd	*ft_process_tokens(t_token *tokens, t_minishell *minishell)
{
	t_cmd *cmd;

	if (ft_check_syntax_errors(tokens))
	{
		minishell->exit_nb = 2;
		ft_gc_remove_list(&minishell->gc_head, tokens);
		minishell->tokens = NULL;
		return (NULL);
	}

	cmd = tokens_to_cmds(tokens, minishell);
	if (!cmd)
	{
		ft_gc_remove_list(&minishell->gc_head, tokens);
		minishell->tokens = NULL;
	}
	return (cmd);
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

	if (!line || !*line)
		return ;
	tokens = NULL;
	cmd = NULL;
	tokens = ft_tokenize(line, minishell);
	if (!tokens)
		return ;
	minishell->tokens = tokens;

	cmd = ft_process_tokens(tokens, minishell);
	if (!cmd)
		return;

	minishell->commands = cmd;
	ft_execute_commands(cmd, minishell);

	ft_gc_remove_list(&minishell->gc_head, tokens);
	ft_gc_remove_cmds(&minishell->gc_head, cmd);
	minishell->tokens = NULL;
	minishell->commands = NULL;
}

/**
 * @brief Exécute la boucle principale du shell
 *
 * @param minishell Structure principale du shell
 */
static void	ft_run_shell_loop(t_minishell *minishell)
{
	char *line;

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

	if (argc > 1)
	{
		ft_putstr_fd("Usage: ./minishell\n", 2);
		return (1);
	}
	(void)argv;
	ft_initialize(&minishell, envp);
	ft_setup_signals();

	ft_run_shell_loop(&minishell);

	ft_clean_exit(&minishell, minishell.exit_nb);
	return (0);
}

