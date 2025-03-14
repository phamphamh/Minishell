/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_echo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 11:42:11 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_echo_options(t_cmd *cmd, int *i)
{
	int	n_opt;

	n_opt = 0;
	while (cmd->args[++(*i)])
	{
		if (ft_strcmp(cmd->args[*i], "-n") == 0)
			n_opt = 1;
		else
			break ;
	}
	return (n_opt);
}

static void	ft_echo_print(t_cmd *cmd, int i, int fd)
{
	int	first;

	first = 1;
	while (cmd->args[i])
	{
		if (!first)
			ft_putchar_fd(' ', fd);
		ft_putstr_fd(cmd->args[i], fd);
		first = 0;
		i++;
	}
}

int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	n_opt;
	int	fd;

	i = 0;
	fd = STDOUT_FILENO;
	if (cmd->pipe_out != -1)
		fd = cmd->pipe_out;
	n_opt = ft_echo_options(cmd, &i);
	ft_echo_print(cmd, i, fd);
	if (!n_opt)
		ft_putchar_fd('\n', fd);
	return (0);
}
