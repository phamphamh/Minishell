/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 11:01:22 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/14 13:00:35 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

static int	word_counter(char const *s, char c)
{
	int	set;
	int	i;
	int	count;

	count = 0;
	i = 0;
	set = 1;
	while (s[i])
	{
		if (s[i] == c)
			set = 1;
		else if (set == 1)
		{
			set = 0;
			count++;
		}
		i++;
	}
	return (count);
}

static int	word_len(char const *s, char c)
{
	int	i;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

static char	*ft_strdup_split(char const *s, char c )
{
	char	*dest;
	int		siz;
	int		i;

	i = 0;
	siz = word_len(s, c);
	dest = malloc(sizeof(char) * (siz + 1));
	if (!dest)
		return (NULL);
	ft_bzero(dest, siz + 1);
	i = 0;
	while (i < siz)
	{
		dest[i] = s[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

static void	ft_free(char **new)
{
	unsigned int	i;

	i = 0;
	while (new[i])
	{
		free(new[i]);
		i++;
	}
	free(new);
}

char	**ft_split(char const *s, char c)
{
	char	**new;
	int		i;
	int		word_count;
	int		word;

	if (!s)
		return (NULL);
	word_count = word_counter(s, c);
	new = (char **)malloc(sizeof(char *) * (word_count + 1));
	if (!new)
		return (NULL);
	word = 0;
	i = -1;
	while (word < word_count)
	{
		if (s[++i] != c)
		{
			new[word++] = ft_strdup_split(&s[i], c);
			if (!new[word - 1])
				return (ft_free(new), NULL);
			i += word_len(&s[i], c);
		}
	}
	return (new[word] = 0, new);
}
