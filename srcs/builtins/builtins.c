/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 21:14:55 by yboumanz          #+#    #+#             */
/*   Updated: 2025/01/15 00:40:11 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/header.h"


/* je verifie si le l'arg d'exit est un nombre et si il y a trop d'arguments,
puis j'exit avec le bon code
s'execute quand meme si l'arg est pas numerique
s'execute pas si l'arg est numerique mais qu'il y a trop d'arguments
*/
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

void	ft_cd(t_minishell *minishell)
{
	if (minishell->tokens->next && minishell->tokens->next->next)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 1);
		// exit_nb = 1;
		return;
	}
	if (!minishell->tokens->next || ft_strcmp_trim(minishell->tokens->next->value, "~") == 0)
	{
		if (chdir(getenv("HOME")) != 0)
		{
			ft_putstr_fd("minishell: cd: ", 1);
			ft_putstr_fd(strerror(errno), 1);
			ft_putchar_fd('\n', 1);
			// exit_nb = 1;
			//revoir les exits
		}
		return;
	}
	if (ft_strcmp_trim(minishell->tokens->next->value, "-") == 0)
	{
		if (chdir(getenv("OLDPWD")) != 0)
		{
			ft_putstr_fd("minishell: cd: ", 1);
			ft_putstr_fd(strerror(errno), 1);
			ft_putchar_fd('\n', 1);
			// exit_nb = 1;
		}
		else
			ft_putstr_fd(getenv("OLDPWD"), 1);
		return;
	}
	if (chdir(minishell->tokens->next->value) != 0)
	{
		ft_putstr_fd("minishell: cd: '", 1);
		ft_putstr_fd(minishell->tokens->next->value, 1);
		ft_putstr_fd("': ", 1);
		ft_putstr_fd(strerror(errno), 1);
		ft_putchar_fd('\n', 1);
		return;
	}
	setenv("OLDPWD", getenv("PWD"), 1);
	setenv("PWD", getcwd(NULL, 0), 1);
}

void	ft_unset(t_minishell *minishell)
{
	t_env	*current;
	t_env	*prev;

	if (!minishell->tokens->next)
	{
		ft_putstr_fd("minishell: unset: missing argument\n", 1);
		return;
	}
	current = minishell->env;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp_trim(current->var, minishell->tokens->next->value) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				minishell->env = current->next;
			free(current->var);
			free(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}
// faire marcher
void	ft_env(t_minishell *minishell)
{
    t_env *temp;

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

void	ft_export(t_minishell *minishell)
{
	t_env	*new_node;
	t_env	*current;

	if (!minishell->tokens->next)
	{
		ft_putstr_fd("minishell: export: missing argument\n", 1);
		return;
	}
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return;
	new_node->var = ft_strdup(minishell->tokens->next->value);
	new_node->next = NULL;
	if (!minishell->env)
		minishell->env = new_node;
	else
	{
		current = minishell->env;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
}

// les index sont set de sorte que seul 1 commande est dans l'input, changer plus tard
// changer les retours d'erreurs, et vérifier si y'a tous les retours d'erreurs.
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

