/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 17:04:23 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/09 14:47:56 by tcousin          ###   ########.fr       */
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

	if (!ptr)
		return (false);
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

	if (!ptr)
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
	t_gc_node	*temp;

	current = *gc_head;
	while (current)
	{
		free(current->ptr);
		temp = current;
		current = current->next;
		free(temp);
	}
	*gc_head = NULL;
}

void	ft_gc_remove_list(t_gc_node **gc_head, t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		ft_gc_remove(gc_head, current->value);
		ft_gc_remove(gc_head, current);
		current = next;
	}
}
