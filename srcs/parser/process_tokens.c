/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by tcousin          #+#    #+#              */
/*   Updated: 2025/03/10 19:11:31 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/*
 * brief: Traite un tableau de chaînes pour créer une liste de tokens
 *
 * split_input: Tableau de chaînes à traiter
 * minishell: Structure minishell pour le garbage collector
 * return: Liste de tokens créée, NULL en cas d'erreur
 */

static int	ft_process_single_token(t_tokenizer *t, char *token, int token_type)
{
	t_token	*new_token;

	new_token = ft_create_token(token, token_type, t->minishell);
	if (!new_token)
		return (0);
	ft_add_token(&t->token_list, new_token);
	t->prev = new_token;
	return (1);
}

static int	ft_should_concat(t_token *prev, int token_type, int skip_concat)
{
	if (prev && prev->type == TOKEN_WORD && token_type == TOKEN_WORD
		&& !skip_concat)
		return (1);
	return (0);
}

void	ft_init_tokenizer(t_tokenizer *t, t_minishell *minishell)
{
	t->prev = NULL;
	t->token_list = NULL;
	t->minishell = minishell;
	t->is_cmd = 1;
	t->skip_concat = 0;
}

/**
 * @brief Traite un token unique
 */
int	ft_process_current_token(t_tokenizer *t, char *token)
{
	int	token_type;

	token_type = ft_determine_token_type(token, &t->is_cmd, t->prev);
	if (ft_handle_space_token(token))
	{
		t->skip_concat = 1;
		return (1);
	}
	if (ft_should_concat(t->prev, token_type, t->skip_concat))
		ft_concat_tokens(t->prev, token, t->minishell);
	else if (!ft_process_single_token(t, token, token_type))
		return (0);
	t->skip_concat = 0;
	return (1);
}

/**
 * @brief Traite l'ensemble des tokens
 */
int	ft_process_token(char **split_input, t_tokenizer *t)
{
	int	i;

	ft_init_tokenizer(t, t->minishell);
	i = 0;
	while (split_input[i])
	{
		if (!ft_process_current_token(t, split_input[i]))
			return (0);
		i++;
	}
	return (1);
}
