/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 09:04:40 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/10 13:07:55 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t				i;
	unsigned const char	*s2;

	s2 = s;
	i = 0;
	while (i < n)
	{
		if (s2[i] == (unsigned char) c)
			return ((char *)&s2[i]);
		i++;
	}
	return (NULL);
}
