/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/15 12:38:30 by tcousin          ###   ########.fr       */
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
	t_env	*head;
	t_env	*new_node;
	t_env	*current;
	int		i;

	head = NULL;
	i = 0;
	while (envp[i])
	{
		new_node = malloc(sizeof(t_env));
		if (!new_node)
			return (NULL);
		ft_gc_add(&minishell->gc_head, new_node);
		new_node->var = ft_strdup(envp[i]);
		if (!new_node->var)
			return (NULL);
		ft_gc_add(&minishell->gc_head, new_node->var);
		new_node->next = NULL;
		if (!head)
			head = new_node;
		else
		{
			current = head;
			while (current->next)
				current = current->next;
			current->next = new_node;
		}
		i++;
	}
	return (head);
}

/*
brief: initialized environnement and garbage collector

arg1: environnement of the programm
arg2:  minishell struct which will contain lexed arg

*/
void	ft_initialize(t_minishell *minishell, char **envp)
{
	minishell->env = ft_env_to_list(envp, minishell);
	minishell->gc_head = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
}

static void	ft_process_line(char *line, t_minishell *minishell)
{
	t_token	*tokens;
	t_cmd	*cmd;

	if (!line || !*line)
		return ;
	add_history(line);
	tokens = ft_tokenize(line, minishell);
	if (!tokens)
		return ;
	if (ft_check_syntax_errors(tokens))
		return;
	cmd = tokens_to_cmds(tokens, minishell);
	if (!cmd)
		return ;
	while (cmd)
	{
		ft_execute_command(cmd, minishell);
		cmd = cmd->next;
	}
}

static void	ft_initialize_shell(t_minishell *minishell)
{
	minishell->env = NULL;
	minishell->gc_head = NULL;
	minishell->tokens = NULL;
	minishell->commands = NULL;
	minishell->exit_nb = 0;
}

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
			ft_clean_exit(&minishell, minishell.exit_nb);
		}
		ft_process_line(line, &minishell);
		free(line);
	}
	return (0);
}
