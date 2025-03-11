/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_part1.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:44:53 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/*
** Exemple de fonction 1.
** Respecte la limite de 25 lignes.
*/
int	ft_util_func1(void)
{
	int	result;

	result = 0;
	result = ft_strlen("dummy");
	return (result);
}

/*
** Exemple de fonction 2.
*/
int	ft_util_func2(int a)
{
	int	res;

	res = a * 2;
	return (res);
}

/*
** Exemple de fonction 3.
*/
int	ft_util_func3(char *str)
{
	int	len;

	len = ft_strlen(str);
	return (len);
}

/*
** Exemple de fonction 4.
*/
int	ft_util_func4(char *s1, char *s2)
{
	char	*tmp;
	int		result;

	tmp = ft_strjoin(s1, s2);
	if (!tmp)
		return (0);
	result = ft_strlen(tmp);
	free(tmp);
	return (result);
}
