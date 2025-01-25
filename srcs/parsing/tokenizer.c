/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 11:44:11 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/25 14:12:51 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/* je recois la valeur, le type, et la liste chainee du token
je complete la boucle en creant le token et en le mettant
dans la liste chainee
*/
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


t_token *ft_tokenize(char *input, t_minishell *minishell)
{
    char    **split_input;
    t_token *token_list;
    t_token *prev_token;
    int     i;
    int     type;

    token_list = NULL;
    prev_token = NULL;
    i = 0;
    split_input = ft_split_with_quotes(input, ' ');
    if (!split_input)
        return (NULL);
    while (split_input[i])
    {
        type = ft_determine_token_type(split_input[i], prev_token);
        if (!ft_integrate_token(split_input[i], type, &token_list, minishell))
            return (ft_free_arrays(split_input), NULL);
        prev_token = token_list;
        while (prev_token && prev_token->next)
            prev_token = prev_token->next;

        i++;
    }
    ft_free_arrays(split_input);
    return (token_list);
}


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
