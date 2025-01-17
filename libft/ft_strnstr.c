/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 09:53:02 by jspitz            #+#    #+#             */
/*   Updated: 2025/01/13 13:57:52 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	count;

	i = 0;
	if (little[0] == '\0')
		return ((char *)big);
	while (big[i] != '\0' && i < len)
	{
		if (big[i] == little[0])
		{
			count = 0;
			while (i + count < len && big[i + count] == little[count]
				&& little[count] != '\0')
				count++;
			if (little[count] == '\0')
				return ((char *)&big[i]);
		}
		i++;
	}
	return (NULL);
}
