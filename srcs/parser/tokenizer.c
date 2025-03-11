/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/10 19:11:31 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/*
 * brief: Crée un nouveau token avec la valeur et le type spécifiés
 *
 * value: Valeur du token
 * type: Type du token
 * minishell: Structure minishell pour le garbage collector
 * return: Nouveau token créé, NULL en cas d'erreur
 */
t_token	*ft_create_token(char *value, int type, t_minishell *minishell)
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

static t_token	*ft_generate_token_list(char **split_input,
		t_minishell *minishell)
{
	t_tokenizer	tokenizer;

	tokenizer.prev = NULL;
	tokenizer.token_list = NULL;
	tokenizer.minishell = minishell;
	tokenizer.is_cmd = 1;
	tokenizer.skip_concat = 0;
	if (!ft_process_token(split_input, &tokenizer))
		return (NULL);
	return (tokenizer.token_list);
}

/*
 * brief: Convertit une ligne d'entrée en liste de tokens
 *
 * input: Ligne d'entrée à tokeniser
 * minishell: Structure minishell pour le garbage collector
 * return: Liste de tokens créée, NULL en cas d'erreur
 */
static char	**ft_filter_tokens(char **split_input, t_minishell *minishell)
{
	char	**filtered_input;
	int		valid_count;
	int		i;
	int		j;

	valid_count = 0;
	i = -1;
	while (split_input[++i])
	{
		if (split_input[i][0] != '\0')
			valid_count++;
	}
	filtered_input = malloc(sizeof(char *) * (valid_count + 1));
	if (!filtered_input)
		return (NULL);
	ft_gc_add(&minishell->gc_head, filtered_input);
	i = -1;
	j = 0;
	while (split_input[++i])
	{
		if (split_input[i][0] != '\0')
			filtered_input[j++] = split_input[i];
	}
	filtered_input[j] = NULL;
	return (filtered_input);
}

static char	**ft_expand_and_split_input(char *input, t_minishell *minishell)
{
	char	*expanded_input;
	char	**split_input;

	expanded_input = ft_expand_operators(input);
	if (!expanded_input)
		return (NULL);
	split_input = ft_split_with_quotes(expanded_input, ' ', minishell);
	free(expanded_input);
	return (split_input);
}

t_token	*ft_tokenize(char *input, t_minishell *minishell)
{
	char	**split_input;
	char	**filtered_input;
	t_token	*token_list;

	split_input = ft_expand_and_split_input(input, minishell);
	if (!split_input)
		return (NULL);
	filtered_input = ft_filter_tokens(split_input, minishell);
	if (!filtered_input)
		return (NULL);
	token_list = ft_generate_token_list(filtered_input, minishell);
	return (token_list);
}
