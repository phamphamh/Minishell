/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_process_line.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/12 13:52:45 by yboumanz         ###   ########.fr       */
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

	if (cmd == NULL)
		return (0);
	last_pid = -1;
	if (ft_is_builtin(cmd->name) && (cmd->next == NULL && cmd->prev == NULL)
		&& cmd->redirs == NULL && !cmd->has_pipe)
	{
		if (!ft_handle_redirection(cmd, cmd->redirs))
			return (1);
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
		return (minishell->exit_nb);
	}
	else
	{
		ft_foreach_cmd(cmd, minishell, &last_pid);
		if (cmd->has_pipe && ft_is_builtin(cmd->name) && ft_strcmp(cmd->name,
				"export") == 0)
		{
			return (ft_wait_child_for_pid(minishell, last_pid));
		}
		return (ft_wait_child_for_pid(minishell, last_pid));
	}
	return (0);
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

int	ft_process_command_line(t_cmd *cmd_list, t_minishell *minishell)
{
	t_cmd	*current;
	pid_t	pid;
	int		status;
	int		is_last;
	int		is_builtin;

	if (!cmd_list)
		return (1);
	current = cmd_list;
	while (current)
	{
		is_last = (current->next == NULL);
		is_builtin = ft_is_builtin(current->args[0]);
		if (is_builtin && !is_in_pipeline(current))
			ft_execute_builtin_command(current, minishell);
		else
		{
			pid = ft_fork_and_execute(current, minishell);
			if (pid == -1)
				return (1);
			if (is_last)
				ft_wait_child(pid, &status, minishell, is_last);
		}
		current = current->next;
	}
	while (waitpid(-1, &status, 0) > 0)
		;
	return (0);
}

// Fonction pour vérifier si une commande fait partie d'un pipeline
int is_in_pipeline(t_cmd *cmd)
{
	return (cmd->pipe_in != -1 || cmd->pipe_out != -1);
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
	ft_process_command_line(cmd, minishell);
	wait_for_processes(pids, cmd_count, minishell);
	ft_gc_remove(&minishell->gc_head, pids);
	free(pids);
	ft_setup_signals();
}
