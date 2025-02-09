/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/09 13:38:23 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

static bool	is_operator(char c)
{
	return (c == '>' || c == '<' || c == '|');
}

static bool	is_double_operator(const char *input, int i)
{
	return ((input[i] == '>' && input[i + 1] == '>') ||
			(input[i] == '<' && input[i + 1] == '<'));
}

static void	add_double_operator(char *new_input, int *j, const char *input, int *i)
{
	if (*j > 0 && new_input[*j - 1] != ' ')
		new_input[(*j)++] = ' ';
	new_input[(*j)++] = input[*i];
	new_input[(*j)++] = input[++(*i)];
	if (input[*i + 1] != ' ')
		new_input[(*j)++] = ' ';
}

static void	add_operator_spaces(char *new_input, int *j, char current, char next)
{
	if (*j > 0 && new_input[*j - 1] != ' ')
		new_input[(*j)++] = ' ';
	new_input[(*j)++] = current;
	if (next != ' ')
		new_input[(*j)++] = ' ';
}

char	*ft_expand_operators(t_minishell *minishell, const char *input)
{
	int		i;
	int		j;
	char	*new_input;

	if (!input)
		return (NULL);
	new_input = malloc(ft_strlen(input) * 3 + 1);
	if (!new_input)
		return (NULL);
	ft_gc_add(&minishell->gc_head, new_input);
	i = -1;
	j = 0;
	while (input[++i])
	{
		if (is_double_operator(input, i))
			add_double_operator(new_input, &j, input, &i);
		else if (is_operator(input[i]))
			add_operator_spaces(new_input, &j, input[i], input[i + 1]);
		else
			new_input[j++] = input[i];
	}
	new_input[j] = '\0';
	return (new_input);
}


