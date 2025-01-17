/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 07:26:00 by jspitz            #+#    #+#             */
/*   Updated: 2025/01/13 13:56:58 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

static size_t	ft_strlen_lcat(char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

size_t	ft_strlcat(char *dst, const char *src, size_t siz)
{
	size_t	i;
	size_t	initial_dst_size;
	size_t	initial_src_size;

	i = 0;
	initial_dst_size = ft_strlen_lcat(dst);
	initial_src_size = ft_strlen_lcat((char *)src);
	if (siz == 0)
		return (initial_src_size);
	if (siz <= initial_dst_size)
		return (initial_src_size + siz);
	i = initial_dst_size;
	while (src[i - initial_dst_size] && i + 1 < siz)
	{
		dst[i] = src[i - initial_dst_size];
		i++;
	}
	if (i < siz)
		dst[i] = '\0';
	return (initial_dst_size + initial_src_size);
}
