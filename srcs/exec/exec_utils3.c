/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/11 19:46:51 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	handle_is_directory(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": is a directory\n", 2);
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}

void	handle_permission_denied(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": Permission denied\n", 2);
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}

void	handle_memory_error(char *cmd_path, t_minishell *minishell)
{
	free(cmd_path);
	ft_putstr_fd("minishell: memory allocation error\n", 2);
	ft_clean_exit(minishell, 1);
}

void	handle_execve_failure(char *cmd_path, t_minishell *minishell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_path, 2);
	ft_putstr_fd(": ", 2);
	perror("");
	free(cmd_path);
	ft_clean_exit(minishell, 126);
}
