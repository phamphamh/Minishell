/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_execute_process.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 11:58:33 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 11:26:14 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static void	ft_close_fds(t_cmd *cmd)
{
	int	fd;

	fd = 3;
	while (fd < 1024)
	{
		if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
		{
			if (cmd->pipe_in == fd || cmd->pipe_out == fd)
			{
				fd++;
				continue ;
			}
			close(fd);
		}
		fd++;
	}
}

static void	ft_check_grep_argument(t_cmd *cmd, t_minishell *minishell)
{
	if (cmd->name && ft_strcmp(cmd->name, "grep") == 0)
	{
		if (!cmd->args[1])
		{
			ft_putstr_fd("grep: argument manquant\n", 2);
			ft_clean_exit(minishell, 1);
		}
	}
}

static void	ft_setup_child_env(t_cmd *cmd, t_minishell *minishell)
{
	ft_reset_signals();
	ft_setup_pipes(cmd);
	ft_close_unused_fds(cmd);
	if (!ft_handle_redirection(cmd, cmd->redirs, false, minishell))
	{
		ft_close_fds(cmd);
		ft_clean_exit(minishell, 1);
	}
	ft_close_fds(cmd);
}

void	ft_execute_child_process(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	ft_setup_child_env(cmd, minishell);
	ft_check_grep_argument(cmd, minishell);
	if (cmd->name && ft_is_builtin(cmd->name))
	{
		exit_code = ft_execute_builtin(cmd, minishell);
		ft_clean_exit(minishell, exit_code);
	}
	else
		ft_execute_child(cmd, minishell);
	exit(EXIT_FAILURE);
}
