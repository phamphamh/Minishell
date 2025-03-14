/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:47:50 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_setup_heredoc(t_cmd *cmd, t_redir_info *redir)
{
	int	heredoc_fd;

	heredoc_fd = ft_handle_heredoc(redir->last_heredoc, cmd, redir->minishell);
	if (heredoc_fd == -1)
		return (0);
	redir->stdin_fd = dup(STDIN_FILENO);
	if (redir->stdin_fd == -1)
	{
		ft_putstr_fd("minishell: dup error\n", 2);
		close(heredoc_fd);
		return (0);
	}
	if (dup2(heredoc_fd, STDIN_FILENO) == -1)
	{
		ft_putstr_fd("minishell: dup2 error\n", 2);
		close(heredoc_fd);
		close(redir->stdin_fd);
		return (0);
	}
	close(heredoc_fd);
	return (1);
}

int	ft_apply_redirections(t_cmd *cmd, t_redir_info *struct_redir)
{
	if (struct_redir->last_heredoc && !ft_setup_heredoc(cmd, struct_redir))
	{
		if (struct_redir->last_out)
			return (1);
		return (0);
	}
	if (struct_redir->last_in && !ft_setup_input_redirection(cmd, struct_redir))
		return (0);
	if (struct_redir->last_out && !ft_setup_output_redirection(cmd,
			struct_redir))
		return (0);
	if (struct_redir->stdin_fd != -1)
		close(struct_redir->stdin_fd);
	return (1);
}
