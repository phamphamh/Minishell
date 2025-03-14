/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_process_line.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:33:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 09:53:10 by yboumanz         ###   ########.fr       */
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

void	ft_print_commands(t_cmd *cmds)
{
	t_cmd	*current;
	int		cmd_index;
	int		arg_index;
	t_redirection *redir;

	current = cmds;
	cmd_index = 0;
	printf("\n🔹 Liste des commandes générées :\n");
	while (current)
	{
		printf("🔹 Commande %d:\n", cmd_index);
		printf("   - Nom: %s\n", current->name ? current->name : "(null)");
		printf("   - Arguments: ");
		if (current->args)
		{
			arg_index = 0;
			while (current->args[arg_index])
			{
				printf("\"%s\" ", current->args[arg_index]);
				arg_index++;
			}
		}
		else
			printf("(null)");
		printf("\n");
		printf("   - Redirections:\n");
		redir = current->redirs;
		while (redir)
		{
			if (redir->type == TOKEN_REDIR_IN && redir->file)
				printf("     ⏩ Input  (<) -> %s\n", redir->file);
			else if (redir->type == TOKEN_REDIR_OUT && redir->file)
				printf("     ⏩ Output (>) -> %s\n", redir->file);
			else if (redir->type == TOKEN_REDIR_APPEND && redir->file)
				printf("     ⏩ Append (>>) -> %s\n", redir->file);
			else if (redir->type == TOKEN_HEREDOC && redir->file)
				printf("     ⏩ Here-Doc (<<) -> %s\n", redir->file);
			redir = redir->next;
		}
		printf("   - Pipe_in: %d, Pipe_out: %d\n", current->pipe_in, current->pipe_out);
		current = current->next;
		cmd_index++;
	}
	printf("🔹 Fin de la liste des commandes\n\n");
}

static int	execute_commands(t_minishell *minishell, t_cmd *cmd)
{
	pid_t	last_pid;

	if (cmd == NULL)
		return (0);
	last_pid = -1;

	// Exécuter la commande principale
	if (ft_is_builtin(cmd->name) && !cmd->has_pipe)
	{
		if (!ft_handle_redirection(cmd, cmd->redirs, true, minishell))
			return (1);
		minishell->exit_nb = ft_execute_builtin(cmd, minishell);
	}
	else
	{
		ft_foreach_cmd(cmd, minishell, &last_pid);
	}

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
	ft_print_commands(cmd);
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
