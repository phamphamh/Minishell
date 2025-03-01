/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/01 16:24:07 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/*
 * brief: Vérifie si un caractère est un opérateur ('|', '<', '>')
 *
 * c: Caractère à vérifier
 * return: 1 si c'est un opérateur, 0 sinon
 */
int	ft_is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

/*
 * brief: Alloue la mémoire pour la chaîne étendue avec des espaces autour des opérateurs
 *
 * input: Chaîne d'entrée à traiter
 * return: Chaîne allouée avec suffisamment d'espace, NULL en cas d'erreur
 */
char	*ft_allocate_expanded(char *input)
{
	int	i;
	int	extra_spaces;

	i = 0;
	extra_spaces = 0;
	while (input[i])
	{
		if ((input[i] == '<' && input[i + 1] == '<')
			|| (input[i] == '>' && input[i + 1] == '>'))
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
	return (malloc(ft_strlen(input) + extra_spaces + 1));
}

/*
 * brief: Remplit la chaîne étendue en ajoutant des espaces autour des opérateurs
 *
 * input: Chaîne d'entrée à traiter
 * expanded: Chaîne de sortie à remplir
 */
void	ft_fill_expanded(char *input, char *expanded)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (input[i])
	{
		if ((input[i] == '<' && input[i + 1] == '<')
			|| (input[i] == '>' && input[i + 1] == '>'))
		{
			expanded[j++] = ' ';
			expanded[j++] = input[i];
			expanded[j++] = input[i + 1];
			expanded[j++] = ' ';
			i += 2;
		}
		else if (ft_is_operator(input[i]))
		{
			expanded[j++] = ' ';
			expanded[j++] = input[i];
			expanded[j++] = ' ';
			i++;
		}
		else
		{
			expanded[j++] = input[i];
			i++;
		}
	}
	expanded[j] = '\0';
}

/*
 * brief: Étend les opérateurs en ajoutant des espaces autour d'eux
 *
 * input: Chaîne d'entrée à traiter
 * return: Nouvelle chaîne avec les opérateurs espacés, NULL en cas d'erreur
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

/*
 * brief: Détermine le type de token basé sur sa valeur
 *
 * token: Valeur du token à analyser
 * return: Type de token (TOKEN_WORD, TOKEN_PIPE, etc.)
 */
int	ft_determine_token_type(char *token)
{
    if (!token || token[0] == '\0')
        return (TOKEN_WORD); // if env var == ""
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
	return (TOKEN_WORD);
}
