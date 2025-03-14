/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp_trim.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 00:07:56 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/14 09:25:59 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	skip_spaces(const char *str, int i)
{
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	return (i);
}

int	ft_strcmp_trim(const char *s1, const char *s2)
{
	int	i1;
	int	i2;

	if (!s1 || !s2)
		return (1);
	i1 = 0;
	i2 = 0;

	// Ignorer les espaces au début
	i1 = skip_spaces(s1, i1);
	i2 = skip_spaces(s2, i2);

	// Comparer les caractères non-espace
	while (s1[i1] && s2[i2])
	{
		if (s1[i1] != s2[i2])
			return ((unsigned char)s1[i1] - (unsigned char)s2[i2]);
		i1++;
		i2++;

		// Ignorer les espaces en milieu de chaîne
		i1 = skip_spaces(s1, i1);
		i2 = skip_spaces(s2, i2);
	}

	// Vérifier si on a atteint la fin des deux chaînes
	return ((unsigned char)s1[i1] - (unsigned char)s2[i2]);
}
