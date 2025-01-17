/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 12:24:15 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/14 08:52:02 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

static int	nb_size(int n)
{
	int	i;
	int	nb;

	i = 1;
	nb = n;
	if (nb == -2147483648)
		return (10);
	if (nb < 0)
	{
		nb = -nb;
		i++;
	}
	while (nb / 10 > 0)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

static char	*ft_strcat_itoa(char *dest, char c)
{
	int	l;

	l = ft_strlen(dest);
	dest[l] = c;
	return (dest);
}

static char	*ft_itoa2(long int nb, char *new)
{
	int		i;
	int		nb2;
	int		size;

	i = 0;
	size = nb_size(nb);
	while (size > 0)
	{
		i = 0;
		nb2 = nb;
		while (i < size - 1)
		{
			nb2 /= 10;
			i++;
		}
		ft_strcat_itoa(new, (nb2 % 10) + '0');
		size--;
	}
	return (new);
}

char	*ft_itoa(int n)
{
	int				size;
	char			*new;
	long int		nb;

	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	if (n == 2147483647)
		return (ft_strdup("2147483647"));
	size = nb_size(n);
	new = malloc(sizeof(char) * size + 1);
	if (!new)
		return (NULL);
	ft_bzero(new, size + 1);
	new[0] = '\0';
	if (n < 0)
	{
		n = (long int)-n;
		ft_strcat_itoa(new, '-');
	}
	nb = n;
	ft_itoa2(nb, new);
	return (new);
}
