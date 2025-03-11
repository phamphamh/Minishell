/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:05:11 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_update_last_redirections(t_redirection *current,
		t_redirection **last_out, t_redirection **last_in,
		t_redirection **last_heredoc)
{
	if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_APPEND)
		*last_out = current;
	else if (current->type == TOKEN_REDIR_IN)
		*last_in = current;
	else if (current->type == TOKEN_HEREDOC)
		*last_heredoc = current;
}

void	ft_find_last_redirections(t_redirection *redir,
		t_redirection **last_out, t_redirection **last_in,
		t_redirection **last_heredoc)
{
	*last_out = NULL;
	*last_in = NULL;
	*last_heredoc = NULL;
	while (redir)
	{
		ft_update_last_redirections(redir, last_out, last_in, last_heredoc);
		redir = redir->next;
	}
}
