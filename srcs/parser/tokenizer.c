/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/15 13:41:42 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static t_token *ft_create_token(char *value, int type, t_minishell *minishell)
{
    t_token *new_token;

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

static t_token *ft_add_token(t_token **token_list, t_token *new_token)
{
    t_token *current;

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

static t_token *ft_process_tokens(char **split_input, t_minishell *minishell)
{
    t_token *token_list;
    t_token *new_token;
    int i;

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

t_token *ft_tokenize(char *input, t_minishell *minishell)
{
    char **split_input;
    char *expanded_input;
    t_token *token_list;

    expanded_input = ft_expand_operators(input);
    if (!expanded_input)
        return (NULL);
    ft_gc_add(&minishell->gc_head, expanded_input);
    split_input = ft_split_with_quotes(expanded_input, ' ', minishell);
    free(expanded_input);
    if (!split_input)
        return (NULL);
    token_list = ft_process_tokens(split_input, minishell);
    return (token_list);
}


