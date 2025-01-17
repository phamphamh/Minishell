/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 09:26:33 by jspitz            #+#    #+#             */
/*   Updated: 2024/11/13 09:18:50 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*new_tab;

	if (nmemb > 0 && size > SIZE_MAX / nmemb)
		return (NULL);
	new_tab = malloc(size * nmemb);
	if (!new_tab)
		return (NULL);
	ft_bzero(new_tab, nmemb * size);
	return (new_tab);
}
