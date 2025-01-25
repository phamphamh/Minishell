/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 21:14:55 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/25 12:07:58 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"

/* Execute la commande exit avec gestion des arguments numériques et des erreurs */
void	ft_exit_cmd(t_minishell *minishell)
{
	ft_putstr_fd("exit\n", 1);
	if (minishell->tokens->next && minishell->tokens->next->type == ARG)
	{
		if (!ft_is_all_nb(minishell->tokens->next->value))
		{
			ft_error_msg("minishell: exit: ", minishell->tokens->next->value,
			": numeric argument required\n", 2);
			ft_clean_exit(minishell, 2);
		}
		if (minishell->tokens->next->next && minishell->tokens->next->next->type == ARG)
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			minishell->exit_nb = 1;
			return ;
		}
		else
			handle_exit_nmb(minishell, ft_atoi(minishell->tokens->next->value));
	}
	else
		ft_clean_exit(minishell, -1);
}

/* Affiche les arguments avec gestion de l'option -n pour supprimer le retour à la ligne */
void	ft_echo_cmd(t_minishell *minishell)
{
	t_token	*current;
	bool	newline;
	bool	first_arg;

	if (!minishell->tokens->next || minishell->tokens->next->type != ARG)
	{
		ft_putchar_fd('\n', 1);
		return;
	}
	first_arg = true;
	newline = true;
	current = minishell->tokens->next;
	while (current && current->type == ARG)
	{
		if (!ft_strcmp(current->value, "-n") && first_arg)
		{
			while (!ft_strcmp(current->value, "-n"))
			{
				if (!current->next)
					return ;
				newline = false;
				current = current->next;
			}
		}
		if (ft_strcmp_trim(current->value, "$?") == 0)
			ft_putnbr_fd(minishell->exit_nb, 1);
		else
			ft_putstr_fd(current->value, 1);
		if (current->next && current->next->type == ARG)
			ft_putchar_fd(' ', 1);
		current = current->next;
		first_arg = false;
	}
	if (newline)
		ft_putchar_fd('\n', 1);
}

/* Affiche le répertoire de travail actuel */
void	ft_pwd(t_minishell *minishell)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		ft_putstr_fd("minishell: pwd: ", 1);
		ft_putstr_fd(strerror(errno), 1);
		ft_putchar_fd('\n', 1);
	}
	else if (minishell->tokens->next)
		ft_putstr_fd("minishell: pwd: too many arguments\n", 1);
	else
	{
		ft_putstr_fd(pwd, 1);
		ft_putchar_fd('\n', 1);
	}
	free(pwd);
}

/* Change le répertoire courant avec gestion de ~, - et des chemins absolus/relatifs */
void	ft_cd(t_minishell *minishell)
{
	if (minishell->tokens->next && minishell->tokens->next->type == ARG
	&& minishell->tokens->next->next && minishell->tokens->next->next->type == ARG)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 1);
		return;
	}
	else if (!minishell->tokens->next || ft_strcmp_trim(minishell->tokens->next->value, "~") == 0)
	{
		if (chdir(getenv("HOME")) != 0)
		{
			ft_putstr_fd("minishell: cd: ", 1);
			ft_putstr_fd(strerror(errno), 1);
			ft_putchar_fd('\n', 1);
		}
	}
	else if (ft_strcmp_trim(minishell->tokens->next->value, "-") == 0)
	{
		if (chdir(getenv("OLDPWD")) != 0)
		{
			ft_putstr_fd("minishell: cd: ", 1);
			ft_putstr_fd(strerror(errno), 1);
			ft_putchar_fd('\n', 1);
		}
		else
			ft_putstr_fd(getenv("OLDPWD"), 1);
	}
	else if (chdir(minishell->tokens->next->value) != 0)
	{
		ft_putstr_fd("minishell: cd: '", 1);
		ft_putstr_fd(minishell->tokens->next->value, 1);
		ft_putstr_fd("': ", 1);
		ft_putstr_fd(strerror(errno), 1);
		ft_putchar_fd('\n', 1);
	}
	update_pwd_and_oldpwd(minishell);
}

/* Supprime une ou plusieurs variables d'environnement */
void	ft_unset(t_minishell *minishell)
{
	t_token	*current_token;

	if (!minishell->tokens->next)
		return ;
	current_token = minishell->tokens->next;
	while (current_token && current_token->type == ARG)
	{
		if (!ft_is_valid_identifier(current_token->value))
		{
			ft_unset_error(current_token->value, minishell);
			current_token = current_token->next;
			continue ;
		}
		ft_handle_unset_var(minishell, current_token->value);
		current_token = current_token->next;
	}
}

/* Affiche toutes les variables d'environnement */
void	ft_env(t_minishell *minishell)
{
	t_env *temp;

	if (minishell->tokens->next && minishell->tokens->next->type == ARG)
	{
		ft_putstr_fd("env : Too many arguments\n", 1);
		return;
	}
	if (!minishell->env)
	{
		ft_putstr_fd("minishell: env: No environment variables\n", 1);
		return;
	}
	temp = minishell->env;
	while (temp)
	{
		ft_putstr_fd("env\n", 1);
		ft_putstr_fd(temp->var, 1);
		ft_putchar_fd('\n', 1);
		temp = temp->next;
	}
}

/* Ajoute ou modifie des variables d'environnement, ou affiche la liste triée si sans arguments */
void	ft_export(t_minishell *minishell)
{
	t_token	*current_token;

	if (!minishell->tokens->next || minishell->tokens->next->type != ARG)
	{
		ft_print_export_list(minishell->env);
		return ;
	}
	current_token = minishell->tokens->next;
	while (current_token && current_token->type == ARG)
	{
		if (!ft_is_valid_identifier_before_equal(current_token->value))
			ft_export_error(current_token->value, minishell);
		else
			ft_handle_export_var(minishell, current_token->value);
		current_token = current_token->next;
	}
}

/* Exécute la commande builtin appropriée en fonction du token */
void	ft_builtins(t_minishell *minishell)
{
	if (!ft_strcmp_trim("exit", minishell->tokens->value))
		ft_exit_cmd(minishell);
	if (!ft_strcmp_trim("echo", minishell->tokens->value))
		ft_echo_cmd(minishell);
	if (!ft_strcmp_trim("pwd", minishell->tokens->value))
		ft_pwd(minishell);
	if (!ft_strcmp_trim("env", minishell->tokens->value))
		ft_env(minishell);
	if (!ft_strcmp_trim("cd", minishell->tokens->value))
		ft_cd(minishell);
	if (!ft_strcmp_trim("unset", minishell->tokens->value))
		ft_unset(minishell);
	if (!ft_strcmp_trim("export", minishell->tokens->value))
		ft_export(minishell);
}

