/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 12:08:32 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:07:31 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_handle_child_signal(int status, t_minishell *minishell, int last_cmd)
{
	if (WIFEXITED(status))
	{
		if (last_cmd)
			minishell->exit_nb = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
		{
			ft_putstr_fd("\n", 1);
			minishell->exit_nb = 130;
		}
		else if (WTERMSIG(status) == SIGQUIT)
		{
			ft_putstr_fd("Quitter (core dumped)\n", 1);
			minishell->exit_nb = 131;
		}
		else
			minishell->exit_nb = 128 + WTERMSIG(status);
	}
}
