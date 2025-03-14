/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:02:19 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/11 11:35:44 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		ss1;
	int		ss2;
	int		i;
	int		a;
	char	*new;

	i = 0;
	a = 0;
	ss1 = ft_strlen(s1);
	ss2 = ft_strlen(s2);
	new = malloc(sizeof(char) * (ss1 + ss2 + 1));
	if (!new)
		return (NULL);
	i = 0;
	a = 0;
	while (s1[i])
	{
		new[i] = s1[i];
		i++;
	}
	while (s2[a])
		new[i++] = s2[a++];
	new[i] = '\0';
	return (new);
}
