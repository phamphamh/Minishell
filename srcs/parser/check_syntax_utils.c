/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:29:45 by tcousin           #+#    #+#             */
/*   Updated: 2025/03/04 14:53:29 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int	ft_special_token_msg(char *value)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(value, 2);
	ft_putstr_fd("'\n", 2);
	return (1);
}

/**
 * @brief Vérifie si un token est spécial et non supporté (&&, ;)
 *
 * @param value Valeur du token à vérifier
 * @return int 1 si erreur de syntaxe, 0 sinon
 */
int	ft_check_special_token(char *value)
{
	if (!value || !value[0])
		return (0);
	if (ft_strcmp(value, "&&") == 0 || ft_strcmp(value, "&&&") == 0
		|| ft_strncmp(value, "&&", 2) == 0)
		return (ft_special_token_msg(value));
	if (ft_strcmp(value, ";") == 0 || ft_strcmp(value, ";;") == 0
		|| ft_strncmp(value, ";", 1) == 0)
		return (ft_special_token_msg(value));
	return (0);
}
