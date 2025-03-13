/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_process_line.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/13 12:45:59 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

static int	count_commands(t_cmd *cmd)
{
	int		count;
	t_cmd	*current;

	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static void	setup_pids(pid_t **pids, int cmd_count, t_minishell *minishell)
{
	int	i;

	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
		return ;
	i = 0;
	while (i < cmd_count)
		(*pids)[i++] = -1;
	ft_gc_add(&minishell->gc_head, *pids);
}

static int	execute_commands(t_minishell *minishell, t_cmd *cmd)
{
	pid_t	last_pid;
	t_cmd	*cmds;
	t_redirection *redir;
	int		heredoc_fd = -1;

	if (cmd == NULL)
		return (0);
	last_pid = -1;

	// 1. Préparer tous les here-docs AVANT d'exécuter quoi que ce soit
	cmds = cmd;
	redir = cmd->redirs;

	while (cmds)
	{
		redir = cmds->redirs;
		while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			heredoc_fd = ft_handle_heredoc(redir);
			if (heredoc_fd == -1) // Si erreur dans le here-doc
				return (1);
		}
		redir = redir->next;
	}
		cmds = cmds->next;
	}

	// 2. Exécuter la commande principale
	if (ft_is_builtin(cmd->name) && !cmd->has_pipe)
	{
		if (!ft_handle_redirection(cmd, cmd->redirs))
			return (1);
		if (heredoc_fd != -1) // Appliquer le here-doc si présent
		{
			dup2(heredoc_fd, STDIN_FILENO);
			close(heredoc_fd); // Fermeture immédiate après duplication
		}
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
	}
	else
	{
		ft_foreach_cmd(cmd, minishell, &last_pid);
	}

	// 3. Attendre la fin des processus et fermer les descripteurs inutiles
	if (heredoc_fd != -1)
		close(heredoc_fd);

	return (ft_wait_child_for_pid(minishell, last_pid));
}

static void	wait_for_processes(pid_t *pids, int cmd_count,
		t_minishell *minishell)
{
	int	i;
	int	status;

	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
			ft_wait_child(pids[i], &status, minishell, i == cmd_count - 1);
		i++;
	}
}

void	ft_process_line(char *line, t_minishell *minishell)
{
	t_token	*tokens;
	t_cmd	*cmd;
	pid_t	*pids;
	int		cmd_count;

	if (!line || !*line)
		return ;
	tokens = ft_tokenize(line, minishell);
	if (!tokens || ft_check_syntax_errors(tokens))
		return ;
	minishell->tokens = tokens;
	cmd = tokens_to_cmds(tokens, minishell);
	if (!cmd)
		return ;
	minishell->commands = cmd;
	cmd_count = count_commands(cmd);
	if (cmd_count == 0)
		return ;
	setup_pids(&pids, cmd_count, minishell);
	execute_commands(minishell, cmd);
	wait_for_processes(pids, cmd_count, minishell);
	ft_gc_remove(&minishell->gc_head, pids);
	free(pids);
	ft_setup_signals();
}
