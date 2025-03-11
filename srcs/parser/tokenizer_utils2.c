/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by tcousin          #+#    #+#              */
/*   Updated: 2025/03/10 19:12:57 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

char	*ft_remove_escape_chars(char *str)
{
	char	*new_str;
	int		i;
	int		j;
	int		len;

	i = 0;
	j = 0;
	len = ft_strlen(str);
	new_str = malloc(len + 1);
	if (!new_str)
		return (NULL);
	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1] && !ft_strchr("$\"\\", str[i + 1]))
			i++;
		new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}

/**
 * @brief Compte les espaces supplémentaires nécessaires pour étendre
 * les opérateurs avec des espaces autour d'eux.
 *
 * @param input Chaîne d'entrée à analyser
 * @return int Nombre d'espaces supplémentaires nécessaires
 */
static int	ft_count_extra_spaces(char *input)
{
	int	i;
	int	extra_spaces;

	i = 0;
	extra_spaces = 0;
	while (input[i])
	{
		if ((input[i] == '<' && input[i + 1] == '<') || (input[i] == '>'
				&& input[i + 1] == '>'))
		{
			extra_spaces += 2;
			i += 2;
		}
		else if (ft_is_operator(input[i]))
		{
			extra_spaces += 2;
			i++;
		}
		else
			i++;
	}
	return (extra_spaces);
}

/**
 * @brief Alloue une nouvelle chaîne avec l'espace nécessaire
 * pour ajouter des espaces autour des opérateurs.
 *
 * @param input Chaîne d'entrée
 * @return char* Chaîne allouée avec la place nécessaire, NULL en cas d'erreur
 */
char	*ft_allocate_expanded(char *input)
{
	int		total_len;
	char	*expanded;

	total_len = ft_strlen(input) + ft_count_extra_spaces(input) + 1;
	expanded = malloc(total_len);
	if (!expanded)
		return (NULL);
	return (expanded);
}

void	ft_fill_expanded(char *input, char *expanded)
{
	t_expand_state	state;
	char			in_quotes;

	state.input = input;
	state.expanded = expanded;
	state.i = (int *)malloc(sizeof(int));
	state.j = (int *)malloc(sizeof(int));
	if (!state.i || !state.j)
	{
		free(state.i);
		free(state.j);
		return ;
	}
	*(state.i) = 0;
	*(state.j) = 0;
	in_quotes = 0;
	while (state.input[*(state.i)])
		ft_process_character(&state, &in_quotes);
	state.expanded[*(state.j)] = '\0';
	free(state.i);
	free(state.j);
}

/**
 * @brief Étend les opérateurs en ajoutant des espaces autour d'eux.
 *
 * @param input Chaîne d'entrée
 * @return char* Nouvelle chaîne avec les opérateurs espacés,
	NULL en cas d'erreur
 */
char	*ft_expand_operators(char *input)
{
	char	*expanded;

	expanded = ft_allocate_expanded(input);
	if (!expanded)
		return (NULL);
	ft_fill_expanded(input, expanded);
	return (expanded);
}
