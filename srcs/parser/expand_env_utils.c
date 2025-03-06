/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by tcousin            #+#    #+#            */
/*   Updated: 2025/03/06 17:05:27 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	process_escape_sequence(const char *str, char *res, int *i, int *j)
{
	if (str[*i] == '\\' && str[*i + 1])
	{
		res[(*j)++] = str[*i + 1];
		*i += 2;
	}
}

char	*prepare_result_buffer(const char *str)
{
	int		len;
	char	*result;

	len = ft_strlen(str) * 2 + 500;
	result = malloc(len);
	if (!result)
		return (NULL);
	return (result);
}
