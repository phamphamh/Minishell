/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:36:12 by yboumanz          #+#    #+#             */
/*   Updated: 2025/03/06 17:58:20 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/**
 * @brief Cherche une sous-chaîne dans une chaîne
 *
 * @param haystack Chaîne dans laquelle chercher
 * @param needle Sous-chaîne à trouver
 * @return char* Pointeur vers le début de la première occurrence,
 *               NULL si non trouvée
 */
char	*ft_strstr(const char *haystack, const char *needle)
{
	size_t	i;
	size_t	j;

	if (!haystack || !needle)
		return (NULL);
	if (!*needle)
		return ((char *)haystack);
	i = 0;
	while (haystack[i])
	{
		j = 0;
		while (needle[j] && haystack[i + j] == needle[j])
			j++;
		if (!needle[j])
			return ((char *)&haystack[i]);
		i++;
	}
	return (NULL);
}

/**
 * @brief Récupère les chemins de recherche pour les commandes
 *
 * @param env Liste des variables d'environnement
 * @return char** Tableau des chemins de recherche, NULL en cas d'erreur
 */
static char	**ft_get_paths(t_env *env)
{
	t_env	*path_var;
	char	**paths;

	path_var = ft_find_env_var(env, "PATH");
	if (!path_var)
		return (NULL);
	paths = ft_split(path_var->var + 5, ':');
	return (paths);
}

/**
 * @brief Trouve le chemin complet d'un exécutable
 *
 * @param cmd_name Nom de la commande
 * @param env Liste des variables d'environnement
 * @return char* Chemin complet de l'exécutable, NULL s'il n'est pas trouvé
 */
char	*ft_find_executable(char *cmd_name, t_env *env)
{
	char	**paths;
	char	*temp;
	char	*cmd_path;
	int		i;

	if (ft_strchr(cmd_name, '/'))
		return (ft_strdup(cmd_name));
	paths = ft_get_paths(env);
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd_name);
		free(temp);
		if (access(cmd_path, X_OK) == 0)
			return (ft_free_arrays(paths), cmd_path);
		free(cmd_path);
		i++;
	}
	ft_free_arrays(paths);
	return (NULL);
}
