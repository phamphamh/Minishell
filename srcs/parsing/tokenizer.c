/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 11:44:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/30 14:57:01 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/* je recois la valeur, le type, et la liste chainee du token
je complete la boucle en creant le token et en le mettant
dans la liste chainee
*/

bool	is_command(char *token, char *prev_token)
{
	if (!prev_token || ft_strcmp(prev_token, "|") == 0)
	{
		if (ft_strcmp(token, "|") != 0 && ft_strcmp(token, ">") != 0 && ft_strcmp(token, "<") != 0)
			return (true);
	}
	return (false);
}

t_token	*ft_create_token(char *value, int type, t_minishell *minishell)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	ft_gc_add(&minishell->gc_head, new_token);
	new_token->value = ft_strdup(value);
	/*
	if (!new_token->value)
	{
		ft_gc_remove(&minishell->gc_head, new_token);
		return (NULL);
	}
	*/
	ft_gc_add(&minishell->gc_head, new_token->value);
	new_token->type = type;
	//printf("value: %s <> type: %d\n", new_token->value, new_token->type);
	new_token->next = NULL;
	return (new_token);
}


bool ft_integrate_token(char *value, int type, t_token **token_list, t_minishell *minishell)
{
    t_token *new_token;
    t_token *current;

    new_token = ft_create_token(value, type, minishell);
    if (!new_token)
        return (false);
    new_token->prev = NULL;
    if (!*token_list)
        *token_list = new_token;
    else
    {
        current = *token_list;
        while (current->next)
            current = current->next;
        current->next = new_token;
        new_token->prev = current;
    }
    return (true);
}

bool is_invalid_token_structure(char *token)
{
    int i;

    if (!token || !token[0])
        return (false);
    if (token[0] == '|' || token[0] == '<' || token[0] == '>')
    {
        i = 1;
        while (token[i])
        {
            if (!(token[i] != '>' && token[i] != '<' && token[i] != '|'))
            {
                ft_putstr_fd("bash: syntax error near unexpected token '", 2);
                ft_putstr_fd(token, 2);
                ft_putstr_fd("'\n", 2);
                return (true);
            }
            i++;
        }
    }
    return (false);
}

t_token *add_token(char *value, int type, t_token **token_list, t_minishell *minishell)
{
    if (!ft_integrate_token(value, type, token_list, minishell))
    {
        ft_putstr_fd("bash: allocation error\n", 2);
        return (NULL);
    }
    return (*token_list);
}

static t_token	*tokenize_and_store(char **split_input, int i, t_token **token_list, t_minishell *minishell)
{
	t_token		*prev_token;
	char		*current_token;
	int			type;

	prev_token = NULL;
	while (split_input[i])
	{
		current_token = split_input[i];
		type = ft_determine_token_type(current_token, prev_token);
		if (!ft_integrate_token(current_token, type, token_list, minishell))
		{
			ft_free_arrays(split_input);
			return (NULL);
		}
		prev_token = *token_list;
		while (prev_token && prev_token->next)
			prev_token = prev_token->next;
		i++;
	}
	return (*token_list);
}

t_token	*ft_tokenize(char *input, t_minishell *minishell)
{
	char	**split_input;
	t_token	*token_list;
	char	*expanded_input;

	token_list = NULL;
	expanded_input = ft_expand_operators(input);
	if (!expanded_input)
		return (NULL);
	split_input = ft_split_with_quotes(expanded_input, ' ');
	free(expanded_input);
	if (!split_input)
		return (NULL);
	token_list = tokenize_and_store(split_input, 0, &token_list, minishell);
	ft_free_arrays(split_input);
	return (token_list);
}
