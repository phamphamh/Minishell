/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 11:53:50 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/14 12:19:07 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	ft_execute(t_minishell *minishell)
{
	if (minishell->tokens->type == CMD && ft_is_builtin(minishell->tokens->value))
	{
		ft_builtins(minishell);
	}
	return ;
}
