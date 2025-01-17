/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 09:26:12 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/30 07:14:36 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"./libft.h"

/**
* @brief print a char
*
* @param c the char to print
*
* @return the number of print char 
*/
int	ft_putchar(char c)
{
	write(1, &c, 1);
	return (1);
}
