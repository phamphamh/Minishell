/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:35:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/27 14:26:10 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

int ft_is_operator(char c)
{
    if (c == '|' || c == '<' || c == '>')
        return (1);
    return (0);
}

char *ft_allocate_expanded(char *input)
{
    int i;
    int count;

    i = 0;
    count = 0;
    while (input[i])
    {
        if (ft_is_operator(input[i]))
            count++;
        i++;
    }
    return (malloc(ft_strlen(input) + (count * 2) + 1));
}

void ft_fill_expanded(char *input, char *expanded)
{
    int i;
    int count;

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
}

char *ft_expand_operators(char *input)
{
    char *expanded;

    expanded = ft_allocate_expanded(input);
    if (!expanded)
        return (NULL);
    ft_fill_expanded(input, expanded);
    return (expanded);
}

int ft_determine_token_type(char *token)
{
    if (!token || token[0] == '\0')
        return (TOKEN_WORD); // if env var == ""
    if (!token)
        return (TOKEN_EOF);
    if (ft_strcmp(token, "|") == 0)
    {
        return (TOKEN_PIPE);
    }
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
