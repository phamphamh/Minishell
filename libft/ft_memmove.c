/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 07:15:51 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/14 08:57:12 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned const char	*csrc;
	unsigned char		*cdest;
	size_t				i;

	if (!dest && !src)
		return (NULL);
	i = -1;
	csrc = src;
	cdest = dest;
	if (cdest > csrc)
	{	
		while (n > 0)
		{
			n--;
			cdest[n] = csrc[n];
		}
	}
	else
		while (++i < n)
			cdest[i] = csrc[i];
	return (dest);
}
