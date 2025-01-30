/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/01/30 14:52:15 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

static bool	is_operator(char c)
{
	return (c == '>' || c == '<' || c == '|');
}

static int	calculate_expanded_length(const char *input)
{
	int	len;
	int	i;

	len = 0;
	i = 0;
	while (input[i])
	{
		if (is_operator(input[i]))
			len += 3;
		else
			len++;
		i++;
	}
	return (len);
}

static void	add_operator_spaces(char *new_input, int *j, char current, char next)
{
	if (*j > 0 && new_input[*j - 1] != ' ')
		new_input[(*j)++] = ' ';
	new_input[(*j)++] = current;
	if (next != ' ')
		new_input[(*j)++] = ' ';
}

char	*ft_expand_operators(const char *input)
{
	int		i;
	int		j;
	char	*new_input;

	if (!input)
		return (NULL);
	new_input = malloc(calculate_expanded_length(input) + 1);
	if (!new_input)
		return (NULL);
	i = -1;
	j = 0;
	while (input[++i])
	{
		if (is_operator(input[i]))
			add_operator_spaces(new_input, &j, input[i], input[i + 1]);
		else
			new_input[j++] = input[i];
	}
	new_input[j] = '\0';
	return (new_input);
}


