/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:25:45 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/14 12:33:12 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

static int	ft_trim_cut(char const *s, char c)
{
	while (*s && c != *s)
		s++;
	return (c == *s);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;

	start = 0;
	end = ft_strlen(s1);
	if (!s1 || !set)
		return (NULL);
	while (ft_trim_cut(set, s1[start]))
		start++;
	if (start == end)
		return (ft_strdup(""));
	while (ft_trim_cut(set, s1[end - 1]))
		end--;
	return (ft_substr(s1, start, (end - start)));
}
