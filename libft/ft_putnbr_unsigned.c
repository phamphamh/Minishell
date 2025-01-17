/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_unsigned.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 09:43:56 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/25 14:32:44 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief even simplier putnbr for no negative to check
		 so no need to check int max problem,
*
* @param nb the unsigned int to print
*
* @return number of character printed
*/
int	ft_putnbr_unsigned_fd(unsigned int nb, int fd)
{
	int	size;

	size = 0;
	if (nb >= 10)
	{
		size += ft_putnbr_unsigned_fd(nb / 10, fd);
	}
	size += ft_putchar_fd(nb % 10 + 48, fd);
	return (size);
}
