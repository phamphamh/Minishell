/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/13 13:40:36 by yboumanz         ###   ########.fr       */
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

t_token	*ft_tokenize(char *input, t_env *env)
{
	t_token	*tokens;
	char	*expanded;
	
	expanded = ft_expand_var(input, env);
	tokens = ft_split_tokens(expanded);
	free(expanded);
	return (tokens);
}

t_token	*ft_split_tokens(char *str)
{
	t_token	*tokens;
	int		i;
	
	tokens = NULL;
	i = 0;
	while (str[i])
	{
		if (ft_isspace(str[i]))
			i++;
		else if (ft_is_operator(&str[i]))
			tokens = ft_add_operator_token(&tokens, &str[i], &i);
		else if (str[i] == '\'' || str[i] == '\"')
			tokens = ft_add_quoted_token(&tokens, &str[i], &i);
		else
			tokens = ft_add_word_token(&tokens, &str[i], &i);
	}
	return (tokens);
}
