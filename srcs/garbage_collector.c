/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:25:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/25 17:47:24 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/*
bref: create and add a new nod to a list

arg1: the list we'll attach the node to
arg2: a pointer to the node to attach

return: true if succed | false if error
*/
bool	ft_gc_add(t_gc_node **gc_head, void *ptr)
{
	t_gc_node	*new_node;
	t_gc_node	*current;

	if (!ptr || !gc_head)
		return (false);
	current = *gc_head;
	while (current)
	{
		if (current->ptr == ptr)
			return (true);
		current = current->next;
	}
	new_node = malloc(sizeof(t_gc_node));
	if (!new_node)
		return (false);
	new_node->ptr = ptr;
	new_node->next = *gc_head;
	*gc_head = new_node;
	return (true);
}

/*
bref: remove a specific given node of a given list

arg1: the list containing the nod to free
arg2: pointer to the node to free

return: true if free succed | fallse if nod is not found
*/
bool	ft_gc_remove(t_gc_node **gc_head, void *ptr)
{
	t_gc_node	*current;
	t_gc_node	*prev;

	if (!gc_head || !*gc_head || !ptr)
		return (false);
	current = *gc_head;
	prev = NULL;
	while (current)
	{
		if (current->ptr == ptr)
		{
			if (prev)
				prev->next = current->next;
			else
				*gc_head = current->next;
			free(current);
			return (true);
		}
		prev = current;
		current = current->next;
	}
	return (false);
}

/*
bref: free pointer + free a list

arg: the list to be free.
*/
void	ft_gc_clear(t_gc_node **gc_head)
{
	t_gc_node	*current;
	t_gc_node	*next;

	if (!gc_head || !*gc_head)
		return ;
	current = *gc_head;
	while (current)
	{
		next = current->next;
		if (current->ptr)
			free(current->ptr);
		free(current);
		current = next;
	}
	*gc_head = NULL;
}

/*
bref: remove tokens from the garbage collector list and free them

arg1: the garbage collector list
arg2: the token list to remove and free
*/
void	ft_gc_remove_list(t_gc_node **gc_head, t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	if (!gc_head || !*gc_head || !tokens)
		return ;
	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
		{
			ft_gc_remove(gc_head, current->value);
		}
		ft_gc_remove(gc_head, current);

		if (current->value)
			free(current->value);
		free(current);

		current = next;
	}
}

/*
bref: remove commands from the garbage collector list and free them

arg1: the garbage collector list
arg2: the command list to remove and free
*/
void	ft_gc_remove_cmds(t_gc_node **gc_head, t_cmd *cmds)
{
	t_cmd	*current;
	t_cmd	*next;
	t_redirection *redir;
	t_redirection *next_redir;
	int i;

	if (!gc_head || !*gc_head || !cmds)
		return ;
	current = cmds;
	while (current)
	{
		next = current->next;

		// Libérer les arguments
		i = 0;
		if (current->args)
		{
			while (current->args[i])
			{
				ft_gc_remove(gc_head, current->args[i]);
				i++;
			}
			ft_gc_remove(gc_head, current->args);
		}

		// Libérer les redirections
		redir = current->redirs;
		while (redir)
		{
			next_redir = redir->next;
			if (redir->file)
				ft_gc_remove(gc_head, redir->file);
			ft_gc_remove(gc_head, redir);
			redir = next_redir;
		}

		// Libérer la commande
		ft_gc_remove(gc_head, current);
		current = next;
	}
}

/*
bref: remove environment variables from the garbage collector list

arg1: the garbage collector list
arg2: the environment variable list to remove
*/
void	ft_gc_remove_env(t_gc_node **gc_head, t_env *env)
{
	t_env	*current;
	t_env	*next;

	if (!gc_head || !*gc_head || !env)
		return ;
	current = env;
	while (current)
	{
		next = current->next;

		// Libérer la variable
		if (current->var)
		{
			ft_gc_remove(gc_head, current->var);
			free(current->var);
		}

		// Libérer le noeud
		ft_gc_remove(gc_head, current);
		free(current);

		current = next;
	}
}

