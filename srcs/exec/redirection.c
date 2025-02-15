/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/15 12:28:16 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

// static int	ft_open_file(char *file, int type)
// {
// 	int	fd;

// 	if (type == TOKEN_REDIR_OUT)
// 		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	else if (type == TOKEN_REDIR_APPEND)
// 		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
// 	else if (type == TOKEN_REDIR_IN)
// 		fd = open(file, O_RDONLY);
// 	else
// 		return (-1);
// 	if (fd == -1)
// 	{
// 		ft_putstr_fd("minishell: ", 2);
// 		ft_putstr_fd(file, 2);
// 		ft_putstr_fd(": No such file or directory\n", 2);
// 		return (-1);
// 	}
// 	return (fd);
// }

int	ft_handle_redirection(t_redirection *redir)
{
	int fd;
	int saved_stdout = dup(STDOUT_FILENO);
	int saved_stdin = dup(STDIN_FILENO);
	t_redirection *last_out = NULL;
	t_redirection *last_in = NULL;

	// Trouver la dernière redirection de sortie et la dernière d'entrée
	t_redirection *current = redir;
	while (current)
	{
		if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_APPEND)
		{
			// Créer tous les fichiers même si seule la dernière sortie est utilisée
			fd = open(current->file, O_CREAT | O_WRONLY, 0644);
			if (fd >= 0)
				close(fd);

			last_out = current;
		}
		else if (current->type == TOKEN_REDIR_IN)
			last_in = current;

		current = current->next;
	}

	// Appliquer la dernière redirection d'entrée si elle existe
	if (last_in)
	{
		fd = open(last_in->file, O_RDONLY);
		if (fd == -1)
		{
			ft_restore_fds(saved_stdin, saved_stdout);
			return (0);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}

	// Appliquer la dernière redirection de sortie si elle existe
	if (last_out)
	{
		fd = open(last_out->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			ft_restore_fds(saved_stdin, saved_stdout);
			return (0);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}

	// Fermer les copies des FDs si inutiles
	if (saved_stdout != -1)
		close(saved_stdout);
	if (saved_stdin != -1)
		close(saved_stdin);

	return (1);
}




void	ft_restore_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}
