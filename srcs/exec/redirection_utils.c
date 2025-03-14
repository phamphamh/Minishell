/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:07:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_update_last_redirections(t_redirection *current,
		t_redirection **last_out, t_redirection **last_in,
		t_redirection **last_heredoc)
{
	if (!current)
		return ;
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

void	ft_restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
			ft_putstr_fd("minishell: dup2 error\n", 2);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
			ft_putstr_fd("minishell: dup2 error\n", 2);
		close(saved_stdout);
	}
}

void	ft_save_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdout = dup(STDOUT_FILENO);
	*saved_stdin = dup(STDIN_FILENO);
	if (*saved_stdout == -1 || *saved_stdin == -1)
		ft_putstr_fd("minishell: dup error\n", 2);
}
