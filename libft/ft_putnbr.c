/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 09:42:14 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/30 07:14:32 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief slightly modified putnbr, it return the nbr of 
*		 character printed
*
* @param n the number to print
*
* @return the number's size
*/
int	ft_putnbr(int n)
{
	int	size;

	size = 0;
	if (n == -2147483648)
	{
		write(1, "-2147483648", 11);
		return (11);
	}
	if (n < 0)
	{
		size += ft_putchar('-');
		n = -n;
	}
	if (n >= 10)
	{
		size += ft_putnbr(n / 10);
	}
	size += ft_putchar(n % 10 + '0');
	return (size);
}
