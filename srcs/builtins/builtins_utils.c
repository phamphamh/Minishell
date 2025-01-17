/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 13:59:09 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/14 12:50:16 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	handle_exit_nmb(t_minishell *minishell, int exit_nmb)
{
	if (exit_nmb > 255 || exit_nmb < 0)
		ft_clean_exit(minishell, exit_nmb % 256);
	else
		ft_clean_exit(minishell, exit_nmb);
}

void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	ft_gc_clear(&minishell->gc_head);
	if (exit_num != -1)
		exit (exit_num);
	else
		exit(minishell->exit_nb);
}

// Fonction dédiée pour afficher les messages d'erreur
void	ft_error_msg(char *prefix, char *arg, char *suffix, int fd)
{
	if (prefix)
		ft_putstr_fd(prefix, fd);
	if (arg)
		ft_putstr_fd(arg, fd);
	if (suffix)
		ft_putstr_fd(suffix, fd);
}



static int	ft_isnum(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

bool	ft_is_all_nb(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isnum(str[i]) && str[i] != '-' && str[i] != '+')
			return (false);
		i++;
	}
	return (true);
}

bool	ft_is_builtin(char *value)
{
	if (!ft_strcmp_trim("exit", value))
		return (true);
	else if (!ft_strcmp_trim("echo", value))
		return (true);
	else if (!ft_strcmp_trim("pwd", value))
		return (true);
	else if (!ft_strcmp_trim("env", value))
		return (true);
	else if (!ft_strcmp_trim("cd", value))
		return (true);
	else if (!ft_strcmp_trim("unset", value))
		return (true);
	else if (!ft_strcmp_trim("export", value))
		return (true);
	else
		return (false);
}
