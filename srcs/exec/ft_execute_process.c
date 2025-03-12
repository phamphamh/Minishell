/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_execute_process.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 11:58:33 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 12:07:00 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_execute_child_process(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	ft_reset_signals();
	ft_setup_pipes(cmd);
	ft_close_unused_fds(cmd);
	if (!ft_handle_redirection(cmd, cmd->redirs))
		ft_clean_exit(minishell, 1);
	if (cmd->name && ft_strcmp(cmd->name, "grep") == 0)
	{
		if (!cmd->args[1])
		{
			ft_putstr_fd("grep: argument manquant\n", 2);
			ft_clean_exit(minishell, 1);
		}
	}
	if (ft_is_builtin(cmd->name))
	{
		exit_code = ft_execute_builtin(cmd, minishell);
		ft_clean_exit(minishell, exit_code);
	}
	else
		ft_execute_child(cmd, minishell);
	exit(EXIT_FAILURE);
}
