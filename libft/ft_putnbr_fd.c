/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 15:15:43 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/25 15:00:19 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<unistd.h>
#include"libft.h"

int	ft_putnbr_fd(int n, int fd)
{
	int	size;

	size = 0;
	if (n == -2147483648)
	{
		write(fd, "-2147483648", 11);
		return (11);
	}
	if (n < 0)
	{
		size += ft_putchar_fd('-', fd);
		n = -n;
	}
	if (n >= 10)
	{
		size += ft_putnbr_fd(n / 10, fd);
	}
	size += ft_putchar_fd(n % 10 + '0', fd);
	return (size);
}
