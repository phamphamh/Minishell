/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 13:59:09 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/10 15:44:54 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* Met à jour une variable d'environnement existante avec une nouvelle valeur */
void	update_env_var(t_env *env, const char *name, const char *new_value, t_minishell *minishell)
{
	t_env	*current;
	size_t	name_len;

	name_len = strlen(name);
	current = env;
	while (current)
	{
		if (strncmp(current->var, name, name_len) == 0 && current->var[name_len] == '=')
		{
			free(current->var);
			current->var = malloc(strlen(name) + strlen(new_value) + 2);
			if (!current->var)
			{
				perror("malloc");
				ft_gc_clear(&minishell->gc_head);
				exit(EXIT_FAILURE);
			}
			sprintf(current->var, "%s=%s", name, new_value);
			return;
		}
		current = current->next;
	}
}

/* Met à jour les variables PWD et OLDPWD après un changement de répertoire */
void	update_pwd_and_oldpwd(t_minishell *minishell)
{
	char	cwd[PATH_MAX];
	t_env	*pwd_var;
	char	*pwd_value;
	
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return;
	}

	pwd_var = ft_find_env_var(minishell->env, "PWD");
	if (pwd_var)
	{
		pwd_value = ft_strchr(pwd_var->var, '=');
		if (pwd_value)
		{
			pwd_value++; // Skip the '='
			update_env_var(minishell->env, "OLDPWD", pwd_value, minishell);
		}
	}
	update_env_var(minishell->env, "PWD", cwd, minishell);
}

/* Gère la normalisation du code de sortie (modulo 256) */
void	handle_exit_nmb(t_minishell *minishell, int exit_nmb)
{
	if (exit_nmb > 255 || exit_nmb < 0)
		ft_clean_exit(minishell, exit_nmb % 256);
	else
		ft_clean_exit(minishell, exit_nmb);
}

/* Affiche un message d'erreur formaté sur le descripteur de fichier spécifié */
void	ft_error_msg(char *prefix, char *arg, char *suffix, int fd)
{
	if (prefix)
		ft_putstr_fd(prefix, fd);
	if (arg)
		ft_putstr_fd(arg, fd);
	if (suffix)
		ft_putstr_fd(suffix, fd);
}

/* Vérifie si un caractère est un chiffre */
static int	ft_isnum(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/* Vérifie si une chaîne ne contient que des chiffres et éventuellement un signe */
bool	ft_is_all_nb(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!ft_isnum(str[i]) && str[i] != '-' && str[i] != '+')
			return (false);
		i++;
	}
	return (true);
}

/* Vérifie si une commande est un builtin */
bool	ft_is_builtin(char *value)
{
	if (!ft_strcmp_trim("exit", value))
		return (true);
	else if (!ft_strcmp_trim("echo", value))
		return (true);
	else if (!ft_strcmp_trim("pwd", value))
		return (true);
	else if (!ft_strcmp_trim("env", value))
		return (true);
	else if (!ft_strcmp_trim("cd", value))
		return (true);
	else if (!ft_strcmp_trim("unset", value))
		return (true);
	else if (!ft_strcmp_trim("export", value))
		return (true);
	else
		return (false);
}

void	ft_print_env(t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		ft_putendl_fd(current->var, 1);
		current = current->next;
	}
}

char	**ft_env_to_array(t_env *env)
{
	t_env	*current;
	char	**env_array;
	int		size;
	int		i;

	size = 0;
	current = env;
	while (current)
	{
		size++;
		current = current->next;
	}
	env_array = malloc(sizeof(char *) * (size + 1));
	if (!env_array)
		return (NULL);
	i = 0;
	current = env;
	while (current)
	{
		env_array[i] = ft_strdup(current->var);
		if (!env_array[i])
		{
			while (i > 0)
				free(env_array[--i]);
			free(env_array);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

void	ft_print_export_list(t_env *env)
{
	t_env	*current;
	char	*equal_pos;

	current = env;
	while (current)
	{
		ft_putstr_fd("declare -x ", 1);
		equal_pos = ft_strchr(current->var, '=');
		if (equal_pos)
		{
			write(1, current->var, equal_pos - current->var + 1);
			ft_putchar_fd('"', 1);
			ft_putstr_fd(equal_pos + 1, 1);
			ft_putchar_fd('"', 1);
		}
		else
			ft_putstr_fd(current->var, 1);
		ft_putchar_fd('\n', 1);
		current = current->next;
	}
}

void	ft_handle_export_var(t_minishell *minishell, char *var)
{
	t_env	*env_var;
	char	*equal_pos;
	char	*var_name;

	if (!ft_is_valid_identifier_before_equal(var))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(var, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return ;
	}
	equal_pos = ft_strchr(var, '=');
	if (!equal_pos)
	{
		env_var = ft_find_env_var(minishell->env, var);
		if (!env_var)
			ft_add_env_var(minishell, var);
		return ;
	}
	var_name = ft_substr(var, 0, equal_pos - var);
	if (!var_name)
		return ;
	env_var = ft_find_env_var(minishell->env, var_name);
	if (env_var)
	{
		free(env_var->var);
		env_var->var = ft_strdup(var);
		if (!env_var->var)
			ft_gc_remove(&minishell->gc_head, env_var);
	}
	else
		ft_add_env_var(minishell, var);
	free(var_name);
}

void	ft_handle_unset_var(t_minishell *minishell, char *var_name)
{
	t_env	*current;
	t_env	*prev;

	if (!ft_is_valid_identifier(var_name))
	{
		ft_putstr_fd("minishell: unset: `", 2);
		ft_putstr_fd(var_name, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return ;
	}
	current = minishell->env;
	prev = NULL;
	while (current)
	{
		if (ft_env_var_match(current->var, var_name))
		{
			if (prev)
				prev->next = current->next;
			else
				minishell->env = current->next;
			ft_gc_remove(&minishell->gc_head, current->var);
			ft_gc_remove(&minishell->gc_head, current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
