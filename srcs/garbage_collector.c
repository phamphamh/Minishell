/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:25:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/02 12:56:22 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

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

bool	ft_gc_remove(t_gc_node **gc_head, void *ptr)
{
	t_gc_node	*cur;
	t_gc_node	*prev;

	if (!gc_head || !*gc_head || !ptr)
		return (false);
	cur = *gc_head;
	prev = NULL;
	while (cur)
	{
		if (cur->ptr == ptr)
		{
			if (prev)
				prev->next = cur->next;
			else
				*gc_head = cur->next;
			free(cur);
			return (true);
		}
		prev = cur;
		cur = cur->next;
	}
	return (false);
}

void	ft_gc_clear(t_gc_node **gc_head)
{
	t_gc_node	*cur;
	t_gc_node	*next;

	if (!gc_head || !*gc_head)
		return ;
	cur = *gc_head;
	while (cur)
	{
		next = cur->next;
		free(cur->ptr);
		free(cur);
		cur = next;
	}
	*gc_head = NULL;
}
