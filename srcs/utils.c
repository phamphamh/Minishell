/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:31:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 16:20:32 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.h"

/**
 * @brief Nettoie les ressources et quitte proprement le programme
 *
 * @param minishell Structure principale à nettoyer
 * @param exit_num Code de sortie à retourner
 */
void	ft_clean_exit(t_minishell *minishell, int exit_num)
{
	// Fermer tous les descripteurs de fichiers ouverts non standards
	ft_close_all_fds(NULL);

	if (minishell)
	{
		if (minishell->tokens)
		{
			ft_gc_remove_list(&minishell->gc_head, minishell->tokens);
			minishell->tokens = NULL;
		}
		if (minishell->commands)
		{
			ft_gc_remove_cmds(&minishell->gc_head, minishell->commands);
			minishell->commands = NULL;
		}
		if (minishell->env)
		{
			ft_gc_remove_env(&minishell->gc_head, minishell->env);
			minishell->env = NULL;
		}
		ft_gc_clear(&minishell->gc_head);
	}
	exit(exit_num);
}

/**
 * @brief Vérifie si une chaîne est un identifiant valide
 *
 * @param str Chaîne à vérifier
 * @return true si c'est un identifiant valide, false sinon
 */
bool	ft_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str || (!ft_isalpha(*str) && *str != '_'))
		return (false);
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Vérifie si la partie avant le '=' est un identifiant valide
 *
 * @param str Chaîne à vérifier
 * @return true si la partie avant '=' est un identifiant valide, false sinon
 */
bool	ft_is_valid_identifier_before_equal(const char *str)
{
	int		i;
	char	*equal_pos;

	if (!str || !*str || (!ft_isalpha(*str) && *str != '_'))
		return (false);
	equal_pos = ft_strchr(str, '=');
	if (!equal_pos)
		return (ft_is_valid_identifier(str));
	i = 1;
	while (str + i < equal_pos)
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Vérifie si une variable d'environnement correspond à un nom donné
 *
 * @param env_var Variable d'environnement à vérifier
 * @param var_name Nom à comparer
 * @return 1 si correspondance, 0 sinon
 */
int	ft_env_var_match(const char *env_var, const char *var_name)
{
	size_t	len;

	if (!env_var || !var_name)
		return (0);
	len = ft_strlen(var_name);
	return (ft_strncmp(env_var, var_name, len) == 0
		&& (env_var[len] == '=' || env_var[len] == '\0'));
}

/**
 * @brief Recherche une variable d'environnement par son nom
 *
 * @param env Liste des variables d'environnement
 * @param var Nom de la variable à rechercher
 * @return t_env* Variable trouvée, NULL si non trouvée
 */
t_env	*ft_find_env_var(t_env *env, const char *var)
{
	t_env	*current;

	if (!env || !var)
		return (NULL);
	current = env;
	while (current)
	{
		if (ft_env_var_match(current->var, var))
			return (current);
		current = current->next;
	}
	return (NULL);
}

/**
 * @brief Ajoute une variable d'environnement
 *
 * @param minishell Structure principale du shell
 * @param var Variable à ajouter (format "NOM=VALEUR")
 */
void	ft_add_env_var(t_minishell *minishell, const char *var)
{
	t_env	*new_var;
	t_env	*current;

	if (!minishell || !var)
		return ;
	new_var = malloc(sizeof(t_env));
	if (!new_var)
		return ;
	ft_gc_add(&minishell->gc_head, new_var);
	new_var->var = ft_strdup(var);
	if (!new_var->var)
	{
		ft_gc_remove(&minishell->gc_head, new_var);
		return ;
	}
	ft_gc_add(&minishell->gc_head, new_var->var);
	new_var->next = NULL;
	if (!minishell->env)
		minishell->env = new_var;
	else
	{
		current = minishell->env;
		while (current->next)
			current = current->next;
		current->next = new_var;
	}
}
