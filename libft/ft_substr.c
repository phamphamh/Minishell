/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 09:46:42 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/14 10:58:33 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	s_size;
	size_t	new_size;
	char	*new;

	i = 0;
	if (!s)
		return (NULL);
	s_size = ft_strlen(s);
	if (s_size <= start)
		return (ft_strdup(""));
	new_size = s_size - start;
	if (new_size > len)
		new_size = len;
	new = (char *)malloc(sizeof(char) * ((new_size + 1)));
	if (!new)
		return (NULL);
	ft_bzero(new, new_size);
	while (i < new_size)
		new[i++] = s[start++];
	new[i] = '\0';
	return (new);
}
