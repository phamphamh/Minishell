/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:17:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/01 16:30:16 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Trouve les dernières redirections de chaque type
 *
 * @param redir Liste des redirections
 * @param last_out Dernière redirection de sortie
 * @param last_in Dernière redirection d'entrée
 * @param last_heredoc Dernière redirection heredoc
 */
static void	ft_find_last_redirections(t_redirection *redir, t_redirection **last_out,
				t_redirection **last_in, t_redirection **last_heredoc)
{
	int				fd;
	t_redirection	*current;

	*last_out = NULL;
	*last_in = NULL;
	*last_heredoc = NULL;
	current = redir;
	while (current)
	{
		if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_REDIR_APPEND)
		{
			if (current->type == TOKEN_REDIR_OUT)
				fd = open(current->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
			else
				fd = open(current->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
			if (fd >= 0)
				close(fd);
			*last_out = current;
		}
		else if (current->type == TOKEN_REDIR_IN)
			*last_in = current;
		else if (current->type == TOKEN_HEREDOC)
			*last_heredoc = current;
		current = current->next;
	}
}

/**
 * @brief Gère les entrées heredoc
 *
 * @param last_heredoc Redirection heredoc à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
static int	ft_handle_heredoc(t_redirection *last_heredoc, int saved_stdin, int saved_stdout)
{
	char	*line;
	int		heredoc_pipe[2];

	if (pipe(heredoc_pipe) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	g_signal_received = 0;
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (g_signal_received)
		{
			free(line);
			break ;
		}
		if (!ft_strcmp(line, last_heredoc->file))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, heredoc_pipe[1]);
		ft_putstr_fd("\n", heredoc_pipe[1]);
		free(line);
	}
	ft_setup_signals();
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);
	return (1);
}

/**
 * @brief Gère l'ouverture et la redirection de l'entrée
 *
 * @param last_in Redirection d'entrée à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
static int	ft_handle_input(t_redirection *last_in, int saved_stdin, int saved_stdout)
{
	int	fd;

	fd = open(last_in->file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_in->file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (1);
}

/**
 * @brief Gère l'ouverture et la redirection de la sortie
 *
 * @param last_out Redirection de sortie à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
static int	ft_handle_output(t_redirection *last_out, int saved_stdin, int saved_stdout)
{
	int	fd;

	if (last_out->type == TOKEN_REDIR_OUT)
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	else
		fd = open(last_out->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_out->file, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (1);
}

/**
 * @brief Gère toutes les redirections d'une commande
 *
 * @param redir Liste des redirections à traiter
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_handle_redirection(t_redirection *redir)
{
	int				saved_stdout;
	int				saved_stdin;
	t_redirection	*last_out;
	t_redirection	*last_in;
	t_redirection	*last_heredoc;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	ft_find_last_redirections(redir, &last_out, &last_in, &last_heredoc);
	if (last_heredoc)
	{
		if (!ft_handle_heredoc(last_heredoc, saved_stdin, saved_stdout))
			return (0);
	}
	else if (last_in)
	{
		if (!ft_handle_input(last_in, saved_stdin, saved_stdout))
			return (0);
	}
	if (last_out)
	{
		if (!ft_handle_output(last_out, saved_stdin, saved_stdout))
			return (0);
	}
	return (1);
}

/**
 * @brief Restaure les descripteurs de fichiers standard
 *
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 */
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
