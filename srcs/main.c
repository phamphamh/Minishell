/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:33:16 by jspitz            #+#    #+#             */
/*   Updated: 2025/02/09 15:44:35 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/header.h"

/*
bref: translate environment variable to chained list

arg: the environnement variable

return : intialized chained list
*/
t_env	*ft_env_to_list(char **envp, t_minishell *minishell)
{
	t_env	*env_list;
	t_env	*new_node;
	t_env	*current;
	int		i;

	env_list = NULL;
	i = -1;
	while (envp[++i])
	{
		new_node = malloc(sizeof(t_env));
		ft_gc_add(&minishell->gc_head, new_node);
		if (!new_node)
			return (NULL);
		new_node->var = ft_strdup(envp[i]);
		if (!new_node->var)
		{
			free(new_node);
			return (NULL);
		}
		ft_gc_add(&minishell->gc_head, new_node->var);
		new_node->next = NULL;
		if (!env_list)
			env_list = new_node;
		else
		{
			current = env_list;
			while (current->next)
				current = current->next;
			current->next = new_node;
		}
	}
	return (env_list);
}

/*
brief: initialized environnement and garbage collector

arg1: environnement of the programm
arg2:  minishell struct which will contain lexed arg

*/
void	ft_initialize(char **envp, t_minishell *minishell)
{
	minishell->gc_head = NULL;
	minishell->exit_nb = 0;
	minishell->env = ft_env_to_list(envp, minishell);
	if (!minishell->env)
	{
		ft_putstr_fd("Error initializing environment\n", 2);
		exit(1);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_minishell minishell = {0};

	(void)argv;
	(void)argc;
	ft_initialize(envp, &minishell);
	while (1)
	{
		input = readline("minishell> ");
		if (!input)
			break ;
		if (input[0] != '\0')
			add_history(input);
		ft_gc_add(&minishell.gc_head, input);
		if (ft_parse(input, &minishell))
		{
			ft_gc_remove(&minishell.gc_head, input);
			continue;
		}
		print_commands(minishell.commands);
		if (!minishell.tokens)
			continue;
		ft_execute(&minishell);
		ft_gc_remove_list(&minishell.gc_head, minishell.tokens);
		ft_gc_remove(&minishell.gc_head, input);
	}

	ft_gc_clear(&minishell.gc_head);
	return (0);
}



