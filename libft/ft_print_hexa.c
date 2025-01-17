/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_hexa.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 09:33:22 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/25 14:34:08 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief like a putnbr but with array hexa lower case or
*		 or uppercase
*
* @param form the flag to say upper or lower
* @param nbr the nbr to print converted in hexa
*
* @return number of char printed
*/
int	ft_print_hexa_fd(char form, long double nbr, int fd)
{
	int		count;
	char	*hexa;

	count = 0;
	hexa = NULL;
	if (form == 'x')
		hexa = "0123456789abcdef";
	else if (form == 'X')
		hexa = "0123456789ABCDEF";
	if (nbr < 0)
	{
		nbr = -nbr;
		write(fd, "-", 1);
		count++;
	}
	if (nbr >= 16)
		count += ft_print_hexa_fd(form, ((unsigned long) nbr / 16), fd);
	ft_putchar_fd(hexa[(unsigned long)nbr % 16], fd);
	count++;
	return (count);
}
