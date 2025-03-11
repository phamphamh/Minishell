/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by tcousin            #+#    #+#            */
/*   Updated: 2025/03/06 17:05:27 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

void	process_escape_sequence(const char *str, char *res, int *i, int *j)
{
	if (str[*i] == '\\' && str[*i + 1])
	{
		res[(*j)++] = str[*i + 1];
		*i += 2;
	}
}

char	*prepare_result_buffer(const char *str, t_minishell *ms)
{
	int		len;
	int		i;
	int		var_len;
	char	*var_name;
	t_env	*env_var;

	len = ft_strlen(str); // Taille initiale de la chaîne
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1]) // Détecte une variable d'env
		{
			i++;
			var_len = extract_var_name(str, i, &var_name);
			if (var_len > 0)
			{
				env_var = ft_find_env_var(ms->env, var_name);
				if (env_var)
					len += ft_strlen(ft_strchr(env_var->var, '=') + 1)
						- var_len;
				free(var_name);
			}
			i += var_len;
		}
		else
			i++;
	}
	return (ft_calloc(len + 1, sizeof(char))); // +1 pour le '\0'
}
