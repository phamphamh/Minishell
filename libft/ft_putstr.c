/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 10:10:11 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/30 07:14:30 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

/**
* @brief print an array of char
*
* @param s the array to print
*
* @return the number of char printed
*/
int	ft_putstr(char *s)
{
	int	i;

	i = 0;
	if (!s)
	{
		return (write(1, "(null)", 6));
	}
	while (s[i] != '\0')
	{
		write (1, &s[i], 1);
		i++;
	}
	return (i);
}
