/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/10 15:51:53 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static int	ft_is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

static char	*ft_expand_operators(char *input)
{
	int		i;
	int		count;
	char	*expanded;

	i = 0;
	count = 0;
	while (input[i])
	{
		if (ft_is_operator(input[i]))
			count++;
		i++;
	}
	expanded = malloc(ft_strlen(input) + (count * 2) + 1);
	if (!expanded)
		return (NULL);
	i = 0;
	count = 0;
	while (input[i])
	{
		if (ft_is_operator(input[i]))
		{
			expanded[count++] = ' ';
			expanded[count++] = input[i];
			expanded[count++] = ' ';
		}
		else
			expanded[count++] = input[i];
		i++;
	}
	expanded[count] = '\0';
	return (expanded);
}

int	ft_determine_token_type(char *token)
{
	if (!token)
		return (TOKEN_EOF);
	if (ft_strcmp(token, "|") == 0)
		return (TOKEN_PIPE);
	if (ft_strcmp(token, "<") == 0)
		return (TOKEN_REDIR_IN);
	if (ft_strcmp(token, ">") == 0)
		return (TOKEN_REDIR_OUT);
	if (ft_strcmp(token, ">>") == 0)
		return (TOKEN_REDIR_APPEND);
	if (ft_strcmp(token, "<<") == 0)
		return (TOKEN_HEREDOC);
	return (TOKEN_WORD);
}

static t_token	*ft_create_token(char *value, int type, t_minishell *minishell)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_gc_add(&minishell->gc_head, new_token);
	new_token->value = ft_strdup(value);
	if (!new_token->value)
	{
		ft_gc_remove(&minishell->gc_head, new_token);
		free(new_token);
		return (NULL);
	}
	ft_gc_add(&minishell->gc_head, new_token->value);
	new_token->type = type;
	new_token->next = NULL;
	new_token->prev = NULL;
	return (new_token);
}

t_token	*ft_tokenize(char *input, t_minishell *minishell)
{
	char	**split_input;
	char	*expanded_input;
	t_token	*token_list;
	t_token	*current;
	t_token	*new_token;
	int		i;

	expanded_input = ft_expand_operators(input);
	if (!expanded_input)
		return (NULL);
	ft_gc_add(&minishell->gc_head, expanded_input);
	split_input = ft_split_with_quotes(expanded_input, ' ', minishell);
	ft_gc_remove(&minishell->gc_head, expanded_input);
	free(expanded_input);
	if (!split_input)
		return (NULL);
	token_list = NULL;
	i = 0;
	while (split_input[i])
	{
		new_token = ft_create_token(split_input[i],
				ft_determine_token_type(split_input[i]), minishell);
		if (!new_token)
		{
			ft_free_arrays(split_input);
			return (NULL);
		}
		if (!token_list)
			token_list = new_token;
		else
		{
			current = token_list;
			while (current->next)
				current = current->next;
			current->next = new_token;
			new_token->prev = current;
		}
		i++;
	}
	ft_free_arrays(split_input);
	return (token_list);
}
