/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:15:23 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/07 13:23:29 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_create_pipe(t_cmd *cmd)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		return (0);
	}
	cmd->pipe_out = pipefd[1];
	if (cmd->next)
		cmd->next->pipe_in = pipefd[0];
	return (1);
}

void	ft_close_pipes(t_cmd *cmd)
{
	if (cmd->pipe_in != -1)
		close(cmd->pipe_in);
	if (cmd->pipe_out != -1)
		close(cmd->pipe_out);
}

void	ft_setup_pipes(t_cmd *cmd)
{
	if (cmd->pipe_in != -1)
	{
		dup2(cmd->pipe_in, STDIN_FILENO);
		close(cmd->pipe_in);
	}
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
	}
} 
