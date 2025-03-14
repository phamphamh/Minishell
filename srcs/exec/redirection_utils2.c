/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 10:41:19 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_init_saved_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdout = -1;
	*saved_stdin = -1;
	ft_save_fds(saved_stdin, saved_stdout);
	if (*saved_stdin == -1 || *saved_stdout == -1)
		return (0);
	return (1);
}

static void	ft_close_fds(int saved_stdin, int saved_stdout, bool restore)
{
	if (restore)
		ft_restore_fds(saved_stdin, saved_stdout);
	else
	{
		close(saved_stdin);
		close(saved_stdout);
	}
}

static int	ft_process_redirections(t_cmd *cmd, t_redirection *redir,
		t_minishell *minishell)
{
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;
	t_redir_info	struct_redir;

	ft_find_last_redirections(redir, &last_out, &last_in, &last_heredoc);
	struct_redir.last_in = last_in;
	struct_redir.last_out = last_out;
	struct_redir.last_heredoc = last_heredoc;
	struct_redir.stdin_fd = -1;
	struct_redir.minishell = minishell;
	return (ft_apply_redirections(cmd, &struct_redir));
}

int	ft_handle_redirection(t_cmd *cmd, t_redirection *redir, bool restore_after,
		t_minishell *minishell)
{
	int saved_stdout;
	int saved_stdin;
	int result;

	if (!redir)
		return (1);
	if (!ft_init_saved_fds(&saved_stdin, &saved_stdout))
		return (0);
	result = ft_process_redirections(cmd, redir, minishell);
	if (!result)
	{
		close(saved_stdin);
		close(saved_stdout);
		return (0);
	}
	ft_close_fds(saved_stdin, saved_stdout, restore_after);
	return (1);
}