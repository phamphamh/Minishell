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
static int	ft_handle_space_token(char *token)
{
	if (ft_strcmp_trim(token, "\1") == 0)
		return (1);
	return (0);
}

static void	ft_concat_tokens(t_token *prev, char *token, t_minishell *minishell)
{
	char	*joined_value;

	joined_value = ft_strjoin(prev->value, token);
	if (!joined_value)
		return ;
	ft_gc_add(&minishell->gc_head, joined_value);
	prev->value = joined_value;
}

static int	ft_process_single_token(t_token **prev, t_token **token_list,
		char *token, int token_type, t_minishell *minishell)
{
	t_token	*new_token;

	new_token = ft_create_token(token, token_type, minishell);
	if (!new_token)
		return (0);
	ft_add_token(token_list, new_token);
	*prev = new_token;
	return (1);
}

static int	ft_should_concat(t_token *prev, int token_type, int skip_concat)
{
	if (prev && prev->type == TOKEN_WORD && token_type == TOKEN_WORD
		&& !skip_concat)
		return (1);
	return (0);
}

static int	ft_process_token(char **split_input, t_minishell *minishell,
		t_token **prev, t_token **token_list)
{
	static int	is_cmd = 1;
	int			skip_concat;
	int			token_type;
	int			i;

	skip_concat = 0;
	i = 0;
	while (split_input[i])
	{
		token_type = ft_determine_token_type(split_input[i], &is_cmd, *prev);
		if (ft_handle_space_token(split_input[i]))
		{
			skip_concat = 1;
			i++;
			continue ;
		}
		if (ft_should_concat(*prev, token_type, skip_concat))
			ft_concat_tokens(*prev, split_input[i], minishell);
		else if (!ft_process_single_token(prev, token_list, split_input[i],
				token_type, minishell))
			return (0);
		skip_concat = 0;
		i++;
	}
	return (1);
}

static t_token	*ft_generate_token_list(char **split_input,
		t_minishell *minishell)
{
	t_token	*token_list;
	t_token	*prev;

	token_list = NULL;
	prev = NULL;
	if (!ft_process_token(split_input, minishell, &prev, &token_list))
		return (NULL);
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
		if ((str[i] != ' ' && str[i] != '\t'))
			// Si ce n'est pas un espace ou un tab
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
static char	**ft_filter_tokens(char **split_input, t_minishell *minishell)
{
	char	**filtered_input;
	int		valid_count;
	int		i;
	int		j;

	valid_count = 0;
	i = 0;
	while (split_input[i])
	{
		if (split_input[i][0] != '\0')
			valid_count++;
		i++;
	}
	filtered_input = malloc(sizeof(char *) * (valid_count + 1));
	if (!filtered_input)
		return (NULL);
	ft_gc_add(&minishell->gc_head, filtered_input);
	i = 0;
	j = 0;
	while (split_input[i])
	{
		if (split_input[i][0] != '\0')
			filtered_input[j++] = split_input[i];
		i++;
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
