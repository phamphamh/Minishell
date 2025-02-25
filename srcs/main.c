/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/25 17:46:39 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/*
bref: translate environment variable to chained list

arg: the environnement variable

return : intialized chained list
*/
t_env	*ft_env_to_list(char **envp, t_minishell *minishell)
{
	t_env	*env;
	t_env	*new;
	t_env	*last;
	char	*var;
	int		i;

	ft_putstr_fd("\n[LOG] Début ft_env_to_list\n", 2);
	env = NULL;
	last = NULL;
	i = 0;
	while (envp[i])
	{
		var = ft_strdup(envp[i]);
		if (!var)
			return (NULL);
		ft_putstr_fd("[LOG] GC_ADD: var=", 2);
		ft_putstr_fd(var, 2);
		ft_putstr_fd("\n", 2);
		if (!ft_gc_add(&minishell->gc_head, var))
		{
			free(var);
			return (NULL);
		}
		new = malloc(sizeof(t_env));
		if (!new)
			return (NULL);
		ft_putstr_fd("[LOG] GC_ADD: new_env_node\n", 2);
		if (!ft_gc_add(&minishell->gc_head, new))
		{
			free(new);
			return (NULL);
		}
		new->var = var;
		new->next = NULL;

		// Ajouter à la fin de la liste plutôt qu'au début
		if (!env)
			env = new;
		else
			last->next = new;
		last = new;
		i++;
	}
	ft_putstr_fd("[LOG] Fin ft_env_to_list\n", 2);
	return (env);
}

/*
brief: initialized environnement and garbage collector

arg1: environnement of the programm
arg2:  minishell struct which will contain lexed arg

*/
void	ft_initialize(t_minishell *minishell, char **envp)
{
	ft_putstr_fd("[LOG] Début ft_initialize\n", 2);
	minishell->env = ft_env_to_list(envp, minishell);
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
	ft_putstr_fd("[LOG] Fin ft_initialize\n", 2);
}

static void	ft_process_line(char *line, t_minishell *minishell)
{
	t_token	*tokens;
	t_cmd	*cmd;

	if (!line || !*line)
		return ;
	add_history(line);
	// Réinitialiser les tokens et commandes pour la nouvelle ligne
	minishell->tokens = NULL;
	minishell->commands = NULL;

	tokens = ft_tokenize(line, minishell);
	if (!tokens)
		return ;
	// Stocker les tokens dans la structure minishell
	minishell->tokens = tokens;
	ft_putstr_fd("[LOG] Tokens créés\n", 2);
	if (ft_check_syntax_errors(tokens))
		return;
	cmd = tokens_to_cmds(tokens, minishell);
	if (!cmd)
		return ;
	// Stocker les commandes dans la structure minishell
	minishell->commands = cmd;
	ft_putstr_fd("[LOG] Commandes créées\n", 2);
	while (cmd)
	{
		ft_execute_command(cmd, minishell);
		cmd = cmd->next;
	}
	ft_putstr_fd("[LOG] Fin ft_process_line\n", 2);
}

static void	ft_initialize_shell(t_minishell *minishell)
{
	ft_putstr_fd("[LOG] Début ft_initialize_shell\n", 2);
	minishell->env = NULL;
	minishell->gc_head = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
	ft_putstr_fd("[LOG] Fin ft_initialize_shell\n", 2);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;
	char		*line;

	(void)argc;
	(void)argv;
	ft_putstr_fd("[LOG] Début du programme\n", 2);
	ft_initialize_shell(&minishell);
	ft_initialize(&minishell, envp);
	ft_setup_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			ft_putstr_fd("[LOG] Sortie (readline NULL)\n", 2);
			ft_clean_exit(&minishell, minishell.exit_nb);
		}
		ft_process_line(line, &minishell);
		free(line);
	}
	return (0);
}
