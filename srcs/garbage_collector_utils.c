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

void	ft_gc_remove_list(t_gc_node **gc_head, t_token *tokens)
{
	t_token	*cur;
	t_token	*next;

	if (!gc_head || !*gc_head || !tokens)
		return ;
	cur = tokens;
	while (cur)
	{
		next = cur->next;
		ft_gc_remove(gc_head, cur->value);
		free(cur->value);
		ft_gc_remove(gc_head, cur);
		free(cur);
		cur = next;
	}
}

void	ft_free_args(t_gc_node **gc_head, char **args)
{
	int	i;

	if (!args)
		return ;
	i = -1;
	while (args[++i])
	{
		if (ft_gc_remove(gc_head, args[i]))
			free(args[i]);
	}
	if (ft_gc_remove(gc_head, args))
		free(args);
}

void	ft_free_redirs(t_gc_node **gc_head, t_redirection *redir)
{
	t_redirection	*next;

	while (redir)
	{
		next = redir->next;
		if (redir->file && ft_gc_remove(gc_head, redir->file))
			free(redir->file);
		if (ft_gc_remove(gc_head, redir))
			free(redir);
		redir = next;
	}
}

void	ft_gc_remove_cmds(t_gc_node **gc_head, t_cmd *cmds)
{
	t_cmd	*cur;
	t_cmd	*next;

	if (!gc_head || !*gc_head || !cmds)
		return ;
	cur = cmds;
	while (cur)
	{
		next = cur->next;
		ft_free_args(gc_head, cur->args);
		ft_free_redirs(gc_head, cur->redirs);
		if (ft_gc_remove(gc_head, cur))
			free(cur);
		cur = next;
	}
}

void	ft_gc_remove_env(t_gc_node **gc_head, t_env *env)
{
	t_env	*cur;
	t_env	*next;

	if (!gc_head || !*gc_head || !env)
		return ;
	cur = env;
	while (cur)
	{
		next = cur->next;
		ft_gc_remove(gc_head, cur->var);
		free(cur->var);
		ft_gc_remove(gc_head, cur);
		free(cur);
		cur = next;
	}
}
