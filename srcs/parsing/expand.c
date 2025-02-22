/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 20:47:50 by tcousin           #+#    #+#             */
/*   Updated: 2025/02/13 13:37:25 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

static bool	is_operator(char c)
{
	return (c == '>' || c == '<' || c == '|');
}

static bool	is_double_operator(const char *input, int i)
{
	return ((input[i] == '>' && input[i + 1] == '>') ||
			(input[i] == '<' && input[i + 1] == '<'));
}

static void	add_double_operator(char *new_input, int *j, const char *input, int *i)
{
	if (*j > 0 && new_input[*j - 1] != ' ')
		new_input[(*j)++] = ' ';
	new_input[(*j)++] = input[*i];
	new_input[(*j)++] = input[++(*i)];
	if (input[*i + 1] != ' ')
		new_input[(*j)++] = ' ';
}

static void	add_operator_spaces(char *new_input, int *j, char current, char next)
{
	if (*j > 0 && new_input[*j - 1] != ' ')
		new_input[(*j)++] = ' ';
	new_input[(*j)++] = current;
	if (next != ' ')
		new_input[(*j)++] = ' ';
}

char	*ft_expand_operators(const char *input)
{
	int		i;
	int		j;
	char	*new_input;

	if (!input)
		return (NULL);
	new_input = malloc(ft_strlen(input) * 3 + 1);
	if (!new_input)
		return (NULL);
	i = -1;
	j = 0;
	while (input[++i])
	{
		if (is_double_operator(input, i))
			add_double_operator(new_input, &j, input, &i);
		else if (is_operator(input[i]))
			add_operator_spaces(new_input, &j, input[i], input[i + 1]);
		else
			new_input[j++] = input[i];
	}
	new_input[j] = '\0';
	return (new_input);
}

char	*ft_expand_var(char *str, t_env *env)
{
	char	*result;
	char	*var_name;
	t_env	*var;
	int		i;
	int		start;

	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && !ft_isspace(str[i + 1]))
		{
			start = ++i;
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				i++;
			var_name = ft_substr(str, start, i - start);
			var = ft_find_env_var(env, var_name);
			free(var_name);
			if (var)
				result = ft_strjoin_free(result, var->var);
		}
		else
			result = ft_strjoin_char_free(result, str[i++]);
	}
	return (result);
}


