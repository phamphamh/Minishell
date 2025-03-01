/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/01 16:24:08 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

char *ft_remove_escape_chars(char *str)
{
    char *new_str;
    int i = 0, j = 0;
    int len = ft_strlen(str);

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

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_gc_add(&minishell->gc_head, new_token);
	new_token->value = ft_strdup(value);
	if (!new_token->value)
		return (NULL);
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
	int		i;

	token_list = NULL;
	i = 0;
	while (split_input[i])
	{
		new_token = ft_create_token(split_input[i],
				ft_determine_token_type(split_input[i]), minishell);
		if (!new_token)
			return (NULL);
		ft_add_token(&token_list, new_token);
		i++;
	}
	return (token_list);
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
    char **split_input;
    char *expanded_input;
    char *cleaned_input;
    t_token *token_list;

    expanded_input = ft_expand_operators(input);
    if (!expanded_input)
        return (NULL);
    ft_gc_add(&minishell->gc_head, expanded_input);

    cleaned_input = ft_remove_escape_chars(expanded_input);
    if (!cleaned_input)
        return (NULL);
    ft_gc_add(&minishell->gc_head, cleaned_input);
    free(expanded_input);

    split_input = ft_split_with_quotes(cleaned_input, ' ', minishell);
    if (!split_input)
        return (NULL);

    token_list = ft_process_tokens(split_input, minishell);
    return (token_list);
}



