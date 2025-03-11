/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 13:31:45 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/11 15:08:13 by yboumanz         ###   ########.fr       */
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
	int	fd;
	t_env *current_env;
	t_env *next_env;

	if (!minishell)
		exit(exit_num);

	// Libérer manuellement l'environnement pour éviter les fuites
	if (minishell->env)
	{
		current_env = minishell->env;
		while (current_env)
		{
			next_env = current_env->next;
			if (current_env->var)
				free(current_env->var);
			free(current_env);
			current_env = next_env;
		}
		minishell->env = NULL;
	}

	// Libérer les tokens
	if (minishell->tokens)
	{
		ft_gc_remove_list(&minishell->gc_head, minishell->tokens);
		minishell->tokens = NULL;
	}

	// Libérer les commandes
	if (minishell->commands)
	{
		ft_gc_remove_cmds(&minishell->gc_head, minishell->commands);
		minishell->commands = NULL;
	}

	// Nettoyer le garbage collector
	if (minishell->gc_head)
	{
		t_gc_node *current = minishell->gc_head;
		t_gc_node *next;

		while (current)
		{
			next = current->next;
			if (current->ptr)
				free(current->ptr);
			free(current);
			current = next;
		}
		minishell->gc_head = NULL;
	}

	// Fermer tous les descripteurs de fichiers ouverts (sauf stdin, stdout, stderr)
	fd = 3;
	while (fd < 1024)
	{
		close(fd);
		fd++;
	}

	// Quitter le programme
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
	size_t	len;
	char	*equal_pos;

	if (!env || !var)
		return (NULL);

	// Déterminer la longueur du nom de la variable (jusqu'au '=' s'il y en a un)
	equal_pos = ft_strchr(var, '=');
	len = equal_pos ? (size_t)(equal_pos - var) : ft_strlen(var);

	current = env;
	while (current)
	{
		// Vérifier si cette variable correspond au nom recherché
		if (ft_strncmp(current->var, var, len) == 0 &&
			(current->var[len] == '=' || current->var[len] == '\0'))
		{
			return (current);
		}
		current = current->next;
	}
	return (NULL);
}

/**
 * @brief Ajoute une variable d'environnement à la liste
 *
 * @param minishell Structure principale du shell
 * @param var Variable à ajouter (format "NOM=VALEUR")
 */
void	ft_add_env_var(t_minishell *minishell, const char *var)
{
	t_env	*new_var;
	t_env	*current;
	char	*env_name;
	size_t	name_len;
	char	*equal_sign;

	if (!minishell || !var)
		return ;

	// Vérifier si la variable existe déjà pour éviter les doublons
	equal_sign = ft_strchr(var, '=');
	if (equal_sign)
	{
		name_len = equal_sign - var;
		env_name = ft_substr(var, 0, name_len);
		if (env_name)
		{
			current = minishell->env;
			while (current)
			{
				if (ft_strncmp(current->var, env_name, name_len) == 0 &&
					(current->var[name_len] == '=' || current->var[name_len] == '\0'))
				{
					// La variable existe déjà, mettre à jour sa valeur
					free(current->var);
					current->var = ft_strdup(var);
					free(env_name);
					return ;
				}
				current = current->next;
			}
			free(env_name);
		}
	}

	// La variable n'existe pas, l'ajouter
	new_var = malloc(sizeof(t_env));
	if (!new_var)
		return ;

	new_var->var = ft_strdup(var);
	if (!new_var->var)
	{
		free(new_var);
		return ;
	}

	new_var->next = NULL;

	// Ajouter à la fin de la liste
	if (!minishell->env)
	{
		minishell->env = new_var;
	}
	else
	{
		current = minishell->env;
		while (current->next)
			current = current->next;
		current->next = new_var;
	}
}
