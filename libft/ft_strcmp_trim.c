/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp_trim.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 00:07:56 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/13 16:03:13 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strlen_read(const char *str)
{
	int	i;
	int	len;

	i = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] != ' ')
			len++;
		i++;
	}
	return (len);
}

int	ft_strcmp_trim(const char *s1, const char *s2)
{
	int	len1;
	int	len2;

	len1 = ft_strlen_read(s1);
	len2 = ft_strlen_read(s2);
	if (len1 != len2)
		return (1);
	return (ft_strncmp(s1, s2, len1));
}
