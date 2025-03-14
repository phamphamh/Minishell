/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Compte le nombre d'arguments dans une séquence de tokens
 *
 * @param start Premier token de la séquence
 * @return int Nombre d'arguments (tokens qui ne sont pas des redirections)
 */
int	ft_count_args(t_token *start)
{
	int		count;
	t_token	*current;

	count = 0;
	current = start;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type != TOKEN_REDIR_IN && current->type != TOKEN_REDIR_OUT
			&& current->type != TOKEN_REDIR_APPEND
			&& current->type != TOKEN_HEREDOC)
			count++;
		current = current->next;
		if (current && (current->type == TOKEN_REDIR_IN
				|| current->type == TOKEN_REDIR_OUT
				|| current->type == TOKEN_REDIR_APPEND
				|| current->type == TOKEN_HEREDOC))
			current = current->next;
	}
	return (count);
}

/**
 * @brief Crée une nouvelle structure de redirection
 *
 * @param token Token de redirection (contient le type)
 * @param next Token suivant (contient le nom du fichier)
 * @param minishell Structure principale du shell
 * @return t_redirection* Nouvelle redirection créée, NULL en cas d'erreur
 */
t_redirection	*ft_create_redirection(t_token *token, t_token *next,
		t_minishell *minishell)
{
	t_redirection	*redir;

	if (!next || !token)
		return (NULL);
	redir = malloc(sizeof(t_redirection));
	if (!redir)
		return (NULL);
	ft_memset(redir, 0, sizeof(t_redirection));
	ft_gc_add(&minishell->gc_head, redir);
	redir->type = token->type;
	redir->file = ft_strdup(next->value);
	if (!redir->file)
	{
		ft_gc_remove(&minishell->gc_head, redir);
		free(redir);
		return (NULL);
	}
	ft_gc_add(&minishell->gc_head, redir->file);
	redir->next = NULL;
	return (redir);
}

/**
 * @brief Gère l'ouverture du fichier en cas de redirection de sortie
 *
 * @param redir Structure de redirection
 */
static void	ft_handle_output_redirection(t_redirection *redir)
{
	int	fd;

	fd = open(redir->file, O_CREAT | O_WRONLY, 0644);
	if (fd >= 0)
		close(fd);
}

/**
 * @brief Ajoute une redirection à une commande
 *
 * @param cmd Commande à modifier
 * @param redir Redirection à ajouter
 */
static void	ft_add_redirection(t_cmd *cmd, t_redirection *redir)
{
	t_redirection	*last_redir;

	if (!cmd->redirs)
		cmd->redirs = redir;
	else
	{
		last_redir = cmd->redirs;
		while (last_redir->next)
			last_redir = last_redir->next;
		last_redir->next = redir;
	}
}

/**
 * @brief Traite une redirection dans la liste de tokens
 *
 * @param current Pointeur vers le token courant (sera avancé)
 * @param cmd Commande à laquelle ajouter la redirection
 * @param minishell Structure principale du shell
 */
void	ft_process_redirection(t_token **current, t_cmd *cmd,
		t_minishell *minishell)
{
	t_redirection	*redir;

	redir = ft_create_redirection(*current, (*current)->next, minishell);
	if (!redir)
		return ;
	if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
		ft_handle_output_redirection(redir);
	ft_add_redirection(cmd, redir);
	if (*current)
		*current = (*current)->next;
	if (*current)
		*current = (*current)->next;
}
