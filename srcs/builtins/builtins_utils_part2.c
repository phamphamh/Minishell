/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_part2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:23:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 18:56:19 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/*
** Exemple de fonction 5.
*/
int	ft_util_func5(void)
{
	int	sum;
	int	i;

	sum = 0;
	i = 0;
	while (i < 10)
	{
		sum += i;
		i++;
	}
	return (sum);
}

/*
** Exemple de fonction 6.
*/
int	ft_util_func6(int a, int b)
{
	int	diff;

	diff = a - b;
	return (diff);
}
