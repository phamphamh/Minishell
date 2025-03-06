/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/04 15:22:06 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Vérifie si un caractère est un opérateur ('|', '<', '>')
 *
 * @param c Caractère à vérifier
 * @return int 1 si c'est un opérateur, 0 sinon
 */

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

/**

	* @brief Remplit la chaîne étendue en
	* ajoutant des espaces autour des opérateurs.
 *
 * @param input Chaîne d'entrée
 * @param expanded Chaîne de sortie allouée
 */
static void	ft_fill_expanded(char *input, char *expanded)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (input[i])
	{
		if ((input[i] == '<' && input[i + 1] == '<') || (input[i] == '>'
				&& input[i + 1] == '>'))
		{
			expanded[j++] = ' ';
			expanded[j++] = input[i++];
			expanded[j++] = input[i++];
			expanded[j++] = ' ';
		}
		else if (ft_is_operator(input[i]))
		{
			expanded[j++] = ' ';
			expanded[j++] = input[i++];
			expanded[j++] = ' ';
		}
		else
			expanded[j++] = input[i++];
	}
	expanded[j] = '\0';
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

/**
 * @brief Détermine le type d'un token à partir de sa valeur.
 *
 * @param token Valeur du token
 * @param prev Token précédent (utile pour HEREDOC)
 * @return int Type du token (TOKEN_WORD, TOKEN_PIPE, etc.)
 */
int	ft_determine_token_type(char *token, t_token *prev)
{
	if (!token)
		return (TOKEN_EOF);
	if (ft_strcmp_trim(token, "|") == 0)
		return (TOKEN_PIPE);
	if (ft_strcmp_trim(token, "<<") == 0)
		return (TOKEN_HEREDOC);
	if (ft_strcmp_trim(token, "<") == 0)
		return (TOKEN_REDIR_IN);
	if (ft_strcmp_trim(token, ">") == 0)
		return (TOKEN_REDIR_OUT);
	if (ft_strcmp_trim(token, ">>") == 0)
		return (TOKEN_REDIR_APPEND);
	if (prev && prev->type == TOKEN_HEREDOC)
		return (TOKEN_EOF);
	return (TOKEN_WORD);
}
