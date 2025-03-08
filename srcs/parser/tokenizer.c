/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/08 13:54:50 by tcousin          ###   ########.fr       */
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

/*
 * brief: Crée un nouveau token avec la valeur et le type spécifiés
 *
 * value: Valeur du token
 * type: Type du token
 * minishell: Structure minishell pour le garbage collector
 * return: Nouveau token créé, NULL en cas d'erreur
 */
static t_token	*ft_create_token(char *value, int type, t_minishell *minishell)
{
	t_token	*new_token;
	char	*cleaned_value;

	cleaned_value = ft_remove_escape_chars(value);
	if (!cleaned_value)
		return (NULL);
	new_token = malloc(sizeof(t_token));
	if (!new_token)
	{
		free(cleaned_value);
		return (NULL);
	}
	ft_gc_add(&minishell->gc_head, new_token);
	new_token->value = cleaned_value;
	ft_gc_add(&minishell->gc_head, new_token->value);
	new_token->type = type;
	new_token->next = NULL;
	new_token->prev = NULL;
	return (new_token);
}

/*
 * brief: Ajoute un token à la fin de la liste de tokens
 *
 * token_list: Adresse de la liste de tokens
 * new_token: Nouveau token à ajouter
 * return: Liste de tokens mise à jour
 */
static t_token	*ft_add_token(t_token **token_list, t_token *new_token)
{
	t_token	*current;

	if (!*token_list)
	{
		*token_list = new_token;
		return (*token_list);
	}
	current = *token_list;
	while (current->next)
		current = current->next;
	current->next = new_token;
	new_token->prev = current;
	return (*token_list);
}

/*
 * brief: Traite un tableau de chaînes pour créer une liste de tokens
 *
 * split_input: Tableau de chaînes à traiter
 * minishell: Structure minishell pour le garbage collector
 * return: Liste de tokens créée, NULL en cas d'erreur
 */
static t_token	*ft_process_tokens(char **split_input, t_minishell *minishell)
{
	t_token	*token_list;
	t_token	*new_token;
	t_token	*prev;
	int		i;
	int		is_cmd;
	int		skip_concat;

	is_cmd = 1;
	skip_concat = 0;
	token_list = NULL;
	prev = NULL;
	i = 0;

	while (split_input[i])
	{
		int token_type = ft_determine_token_type(split_input[i], &is_cmd, prev);

		// 🔹 Si le token courant est "\1" (marqueur espace), on bloque la concaténation
		if (ft_strcmp_trim(split_input[i], "\1") == 0)
		{
			skip_concat = 1;
			i++;
			continue;
		}

		// 🔹 Si c'est un TOKEN_WORD et que le précédent était aussi un TOKEN_WORD (et pas un marqueur espace)
		if (prev && prev->type == TOKEN_WORD && token_type == TOKEN_WORD && !skip_concat)
		{
			char *joined_value = ft_strjoin(prev->value, split_input[i]); // Concatène sans espace
			ft_gc_add(&minishell->gc_head, joined_value);
			prev->value = joined_value;
		}
		else
		{
			// 🔹 Crée un nouveau token
			new_token = ft_create_token(split_input[i], token_type, minishell);
			if (!new_token)
				return (NULL);
			ft_add_token(&token_list, new_token);
			prev = new_token;
		}

		skip_concat = 0; // Reset la concaténation après un espace
		i++;
	}

	return (token_list);
}

int	ft_str_only_spaces(char *str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (1); // Considéré comme vide

	i = 0;
	while (str[i])
	{
		if ((str[i] != ' ' && str[i] != '\t')) // Si ce n'est pas un espace ou un tab
			return (0);
		i++;
	}
	return (1); // Tous les caractères sont des espaces/tabs
}


/*
 * brief: Convertit une ligne d'entrée en liste de tokens
 *
 * input: Ligne d'entrée à tokeniser
 * minishell: Structure minishell pour le garbage collector
 * return: Liste de tokens créée, NULL en cas d'erreur
 */
t_token	*ft_tokenize(char *input, t_minishell *minishell)
{
	char	**split_input;
	char	*expanded_input;
	t_token	*token_list;
	int		i;
	int		j;
	char	**filtered_input;

	i = 0;
	expanded_input = ft_expand_operators(input);
	if (!expanded_input)
		return (NULL);
	split_input = ft_split_with_quotes(expanded_input, ' ', minishell);

	free(expanded_input);
	if (!split_input)
		return (NULL);

	// 🔹 Compter le nombre de tokens valides (non vides)
	int valid_count = 0;
	for (i = 0; split_input[i]; i++)
	{
		if(split_input[i][0] != '\0')
			valid_count++;

	}

	// 🔹 Allouer un nouveau tableau filtré
	filtered_input = malloc(sizeof(char *) * (valid_count + 1));
	if (!filtered_input)
		return (NULL);
	ft_gc_add(&minishell->gc_head, filtered_input);

	// 🔹 Remplir filtered_input avec les tokens valides
	i = 0;
	j = 0;
	while (split_input[i])
	{
		if (split_input[i][0] != '\0')
		{
			filtered_input[j++] = split_input[i];
		}
		i++;
	}
	filtered_input[j] = NULL;

	token_list = ft_process_tokens(filtered_input, minishell);
	return (token_list);
}



