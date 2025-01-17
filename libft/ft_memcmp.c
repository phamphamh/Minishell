/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 09:35:34 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/12 13:01:43 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t				i;
	const unsigned char	*sc1;
	const unsigned char	*sc2;

	sc1 = (const unsigned char *)s1;
	sc2 = (const unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (sc1[i] != sc2[i])
			return ((int)sc1[i] - (int)sc2[i]);
		i++;
	}
	return (0);
}
