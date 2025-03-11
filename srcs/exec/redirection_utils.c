/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 19:05:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 19:39:24 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static void	ft_check_redir(t_redirection *r)
{
	int	fd;

	if (r->type == TOKEN_REDIR_OUT)
		fd = open(r->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else
		fd = open(r->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd >= 0)
		close(fd);
}

void	ft_find_last_redirections(t_redirection *redir,
		t_redirection **last_out, t_redirection **last_in,
		t_redirection **last_heredoc)
{
	t_redirection	*cur;

	*last_out = NULL;
	*last_in = NULL;
	*last_heredoc = NULL;
	cur = redir;
	while (cur)
	{
		if (cur->type == TOKEN_REDIR_OUT || cur->type == TOKEN_REDIR_APPEND)
		{
			ft_check_redir(cur);
			*last_out = cur;
		}
		else if (cur->type == TOKEN_REDIR_IN)
			*last_in = cur;
		else if (cur->type == TOKEN_HEREDOC)
			*last_heredoc = cur;
		cur = cur->next;
	}
}

int	ft_check_input_files(t_redirection *redir,
		int saved_stdin, int saved_stdout)
{
	t_redirection	*cur;
	int				fd;

	cur = redir;
	while (cur)
	{
		if (cur->type == TOKEN_REDIR_IN)
		{
			fd = open(cur->file, O_RDONLY);
			if (fd == -1)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(cur->file, 2);
				ft_putstr_fd(": No such file or directory\n", 2);
				ft_restore_fds(saved_stdin, saved_stdout);
				return (0);
			}
			close(fd);
		}
		cur = cur->next;
	}
	return (1);
}
