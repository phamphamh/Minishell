/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:21:45 by tcousin          #+#    #+#              */
/*   Updated: 2025/03/11 15:05:48 by tcousin         ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	close_pipe_fds(t_cmd *cmd)
{
	if (cmd->pipe_in != -1)
	{
		close(cmd->pipe_in);
		cmd->pipe_in = -1;
	}
	if (cmd->pipe_out != -1)
	{
		close(cmd->pipe_out);
		cmd->pipe_out = -1;
	}
}

void	ft_close_all_pipes(t_cmd *cmd_first)
{
	t_cmd	*current;

	current = cmd_first;
	while (current)
	{
		close_pipe_fds(current);
		current = current->next;
	}
}

char	**ft_allocate_paths(char *path_value)
{
	char	**paths;

	paths = ft_split(path_value, ':');
	if (!paths)
		return (NULL);
	return (paths);
}

void	free_paths(char **paths, int i)
{
	while (i > 0)
		free(paths[--i]);
	free(paths);
}

char	**ft_default_paths(void)
{
	char	**paths;
	int		i;

	paths = malloc(sizeof(char *) * 7);
	if (!paths)
		return (NULL);
	paths[0] = ft_strdup("/usr/local/bin");
	paths[1] = ft_strdup("/usr/bin");
	paths[2] = ft_strdup("/bin");
	paths[3] = ft_strdup("/usr/sbin");
	paths[4] = ft_strdup("/sbin");
	paths[5] = ft_strdup(".");
	paths[6] = NULL;
	i = 0;
	while (i < 6)
	{
		if (!paths[i])
		{
			free_paths(paths, i);
			return (NULL);
		}
		i++;
	}
	return (paths);
}
