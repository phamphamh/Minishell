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

/**
 * @brief Ajoute un pointeur au garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
 * @param ptr Pointeur à ajouter
 * @return true si réussi, false en cas d'erreur
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

/**
 * @brief Retire un pointeur du garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
 * @param ptr Pointeur à retirer
 * @return true si réussi, false si le pointeur n'est pas trouvé
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

/**
 * @brief Libère tous les pointeurs du garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
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

/**
 * @brief Retire et libère une liste de tokens du garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
 * @param tokens Liste de tokens à libérer
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

/**
 * @brief Retire et libère une liste de commandes du garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
 * @param cmds Liste de commandes à libérer
 */
void	ft_gc_remove_cmds(t_gc_node **gc_head, t_cmd *cmds)
{
	t_cmd			*current;
	t_cmd			*next;
	t_redirection	*redir;
	t_redirection	*next_redir;
	int				i;

	if (!gc_head || !*gc_head || !cmds)
		return ;
	current = cmds;
	while (current)
	{
		next = current->next;
		i = 0;
		if (current->args)
		{
			while (current->args[i])
			{
				ft_gc_remove(gc_head, current->args[i]); // ✅ Ajout
				free(current->args[i]); // ✅ Libérer chaque argument
				i++;
			}
			ft_gc_remove(gc_head, current->args);
			free(current->args);
		}
		// Libérer les redirections
		redir = current->redirs;
		while (redir)
		{
			next_redir = redir->next;
			if (redir->file)
			{
				ft_gc_remove(gc_head, redir->file);
				free(redir->file);
			}
			ft_gc_remove(gc_head, redir);
			free(redir);
			redir = next_redir;
		}
		ft_gc_remove(gc_head, current);
		free(current);
		current = next;
	}
}


/**
 * @brief Retire et libère une liste de variables d'environnement du garbage collector
 *
 * @param gc_head Tête de la liste du garbage collector
 * @param env Liste de variables d'environnement à libérer
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
		if (current->var)
		{
			ft_gc_remove(gc_head, current->var);
			free(current->var);
		}
		ft_gc_remove(gc_head, current);
		free(current);
		current = next;
	}
}

