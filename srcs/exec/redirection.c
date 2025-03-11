/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:38:57 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_prepare_redir(t_cmd *cmd, t_redirection *redir,
		int *saved_stdin, int *saved_stdout,
		t_redirection **last_out, t_redirection **last_in,
		t_redirection **last_heredoc)
{
	if (!redir)
		return (1);
	*saved_stdout = dup(STDOUT_FILENO);
	if (*saved_stdout == -1)
	{
		ft_putstr_fd("minishell: dup error\n", 2);
		return (0);
	}
	*saved_stdin = dup(STDIN_FILENO);
	if (*saved_stdin == -1)
	{
		close(*saved_stdout);
		ft_putstr_fd("minishell: dup error\n", 2);
		return (0);
	}
	ft_find_last_redirections(redir, last_out, last_in, last_heredoc);
	if (!ft_check_input_files(redir, *saved_stdin, *saved_stdout))
		return (0);
	return (1);
}

/*
** ft_apply_redir:
** - Applique la redirection heredoc, puis d'entrée et enfin de sortie.
** Retourne 1 si tout s'est bien passé, 0 sinon.
*/
static int	ft_apply_redir(t_cmd *cmd, int saved_stdin, int saved_stdout,
		t_redirection *last_heredoc, t_redirection *last_in,
		t_redirection *last_out)
{
	int	result;

	result = 1;
	if (last_heredoc)
	{
		if (!ft_handle_heredoc(last_heredoc, saved_stdin, saved_stdout))
			result = 0;
	}
	else if (last_in)
	{
		if (!ft_handle_input(cmd, last_in, saved_stdin, saved_stdout))
			result = 0;
	}
	if (result && last_out)
	{
		if (!ft_handle_output(cmd, last_out, saved_stdin, saved_stdout))
			result = 0;
	}
	return (result);
}

/*
** ft_handle_redirection:
** Fonction principale qui prépare, applique et finalise les redirections.
*/
int	ft_handle_redirection(t_cmd *cmd, t_redirection *redir)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;
	int				result;

	if (!ft_prepare_redir(cmd, redir, &saved_stdin, &saved_stdout,
			&last_out, &last_in, &last_heredoc))
		return (0);
	result = ft_apply_redir(cmd, saved_stdin, saved_stdout,
			last_heredoc, last_in, last_out);
	if (!result)
	{
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	close(saved_stdin);
	close(saved_stdout);
	return (1);
}
