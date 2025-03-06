/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/01 16:24:14 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Définit le nom de la commande en fonction des redirections
 *
 * @param cmd Commande à analyser
 * @param minishell Structure principale du shell
 */
void	ft_set_command_name(t_cmd *cmd, t_minishell *minishell)
{
	if (cmd->redirs && cmd->redirs->type == TOKEN_HEREDOC)
	{
		cmd->name = ft_strdup("heredoc");
		ft_gc_add(&minishell->gc_head, cmd->name);
	}
	else
		cmd->name = cmd->args[0];
}

int	ft_is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}
