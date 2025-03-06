/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:36:26 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
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
void	ft_find_last_redirections(t_redirection *redir, t_redirection **last_out,
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
 * @brief Lit les lignes pour un heredoc jusqu'au délimiteur
 *
 * @param last_heredoc Redirection heredoc en cours de traitement
 * @param heredoc_pipe Pipe pour écrire les données
 * @return int 1 si ok, 0 si interruption (Ctrl+C)
 */
static int ft_read_heredoc_lines(t_redirection *last_heredoc, int *heredoc_pipe)
{
	char *line;

	while (1)
	{
		line = readline("> ");
		if (!line)  // L'utilisateur a fait Ctrl+D
		{
			ft_putstr_fd("minishell: warning: here-document ", 2);
			ft_putstr_fd("delimited by end-of-file (wanted `", 2);
			ft_putstr_fd(last_heredoc->file, 2);
			ft_putstr_fd("')\n", 2);
			break;
		}
		if (g_signal_received) // L'utilisateur a fait Ctrl+C
			return (free(line), 0);
		if (!ft_strcmp_trim(line, last_heredoc->file))
		{
			free(line);
			break;
		}
		ft_putstr_fd(line, heredoc_pipe[1]);
		ft_putstr_fd("\n", heredoc_pipe[1]);
		free(line);
	}
	return (1);
}

/**
 * @brief Gère les entrées heredoc
 *
 * @param last_heredoc Redirection heredoc à traiter
 * @param saved_stdin Descripteur de fichier d'entrée sauvegardé
 * @param saved_stdout Descripteur de fichier de sortie sauvegardé
 * @return int 1 si réussi, 0 en cas d'erreur
 */
int	ft_handle_heredoc(t_redirection *last_heredoc, int saved_stdin, int saved_stdout)
{
	int		heredoc_pipe[2];
	int		result;

	if (pipe(heredoc_pipe) == -1)
	{
		ft_putstr_fd("minishell: pipe error\n", 2);
		ft_restore_fds(saved_stdin, saved_stdout);
		return (0);
	}
	ft_heredoc_signals();  // Utiliser notre gestionnaire spécifique pour heredoc
	g_signal_received = 0;

	result = ft_read_heredoc_lines(last_heredoc, heredoc_pipe);

	ft_setup_signals(); // Restaurer les gestionnaires de signaux
	close(heredoc_pipe[1]);
	dup2(heredoc_pipe[0], STDIN_FILENO);
	close(heredoc_pipe[0]);

	if (g_signal_received)
		return (0); // Retourner 0 en cas de Ctrl+C
	else
		return (1); // Retourner 1 sinon
}
