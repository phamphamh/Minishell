/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fdprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 14:35:37 by jspitz            #+#    #+#             */
/*   Updated: 2024/12/25 14:36:04 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

int	ft_fdprintf(int fd, const char *s, ...)
{
	va_list	arg;
	int		i;
	int		nb;

	if (!s)
		return (-1);
	va_start(arg, s);
	i = 0;
	nb = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '%')
			nb += form_conv(s[i++ + 1], arg, fd);
		else
			nb += ft_putchar_fd(s[i], fd);
		i++;
	}
	return (va_end(arg), nb);
}
