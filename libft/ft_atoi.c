/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 23:38:31 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/10 13:06:53 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

int	ft_atoi(const char *nptr)
{
	int	r;
	int	i;
	int	pn;

	i = 0;
	r = 0;
	pn = 1;
	while (nptr[i] == 32 || (nptr[i] > 8 && nptr[i] < 14))
		i++;
	if (nptr[i] == 43 || nptr[i] == 45)
	{	
		if (nptr[i++] == 45)
			pn = -1;
	}
	while (nptr[i] > 47 && nptr[i] < 58)
	{
		r = r * 10 + (nptr[i] - 48);
		i++;
	}
	if (pn == 1)
		return (r);
	else
		return (-r);
}
