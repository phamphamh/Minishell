/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pointer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 09:37:24 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/25 14:21:48 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief print the adress casted in unsigned long in
		 hexa form so it use the / 16 and % 16 operation
		 the (char) nb - 10 + 'a' to print the letter
		 lowercase in the ascii table
*
* @param nb the nb to print in hexa
*
* @return 1 by printed char
*/
static int	ft_printadd(const unsigned long nb, int fd)
{
	if (nb / 16)
		return (ft_printadd(nb / 16, fd) + ft_printadd(nb % 16, fd));
	else if (!(nb / 10))
		ft_putchar (nb + '0');
	else
		ft_putchar_fd((char) nb - 10 + 'a', fd);
	return (1);
}

/**
* @brief check if there is an address to print and
		 ad the "0x" if it does or just print the
*		the error "(nil)" 
*
* @param add the address to print
*
* @return the number of character printed
*/
int	ft_pointer(void *add, int fd)
{
	if (!add)
		return (ft_putstr_fd("(nil)", fd));
	ft_putstr("0x");
	return (2 + ft_printadd((unsigned long) add, fd));
}
