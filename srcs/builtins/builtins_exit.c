/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:29:11 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_exit(t_cmd *cmd, t_minishell *minishell)
{
	int	exit_code;

	ft_putendl_fd("exit", 2);
	if (!cmd->args[1])
		exit_code = minishell->exit_nb;
	else
	{
		if (!ft_is_all_nb(cmd->args[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd->args[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			exit_code = 2;
		}
		else
		{
			exit_code = ft_atoi(cmd->args[1]);
			if (cmd->args[2])
				return (ft_putstr_fd("minishell: exit: too many arguments\n",
						2), 1);
		}
	}
	ft_clean_exit(minishell, exit_code);
	return (0);
}
