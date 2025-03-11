/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:19:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 13:45:38 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

void	free_env_list(t_minishell *minishell)
{
	t_env	*current;
	t_env	*next;

	if (!minishell->env)
		return ;
	current = minishell->env;
	while (current)
	{
		next = current->next;
		free(current->var);
		free(current);
		current = next;
	}
	minishell->env = NULL;
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
		free(current->ptr);
		free(current);
		current = next;
	}
	minishell->gc_head = NULL;
}

void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	int fd;

	if (!minishell)
		exit(exit_num);
	free_env_list(minishell);
	ft_gc_remove_list(&minishell->gc_head, minishell->tokens);
	ft_gc_remove_cmds(&minishell->gc_head, minishell->commands);
	free_gc_list(minishell);
	fd = 3;
	while (fd < 1024)
	{
		close(fd);
		fd++;
	}
	exit(exit_num);
}