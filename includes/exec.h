/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 10:09:48 by yboumanz          #+#    #+#             */
/*   Updated: 2025/02/07 12:30:59 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include <stdbool.h>

typedef struct s_minishell t_minishell;

// exec.c
void	ft_execute(t_minishell *minishell);

//builtins.c
void	ft_exit_cmd(t_minishell *minishell);
void	ft_echo_cmd(t_minishell *minishell);
void	ft_pwd(t_minishell *minishell);
void	ft_cd(t_minishell *minishell);
void	ft_unset(t_minishell *minishell);
void	ft_env(t_minishell *minishell);
void	ft_export(t_minishell *minishell);
void	ft_builtins(t_minishell *minishell);

//builtins_utils.c
bool	ft_is_builtin(char *value);
bool	ft_is_all_nb(char *str);
void	ft_error_msg(char *prefix, char *arg, char *suffix, int fd);
void	ft_clean_exit(t_minishell *minishell, int exit_num);
void	handle_exit_nmb(t_minishell *minishell, int exit_nmb);
void	update_env_var(t_env *env, const char *name, const char *new_value, t_minishell *minishell);
void	update_pwd_and_oldpwd(t_minishell *minishell);

//builtins_utils2.c
void	ft_handle_unset_var(t_minishell *minishell, char *var_name);
void	ft_handle_export_var(t_minishell *minishell, char *var);
void	ft_print_export_list(t_env *env);
void	ft_print_export_var(t_env *env_var);
void	ft_export_error(char *var, t_minishell *minishell);
void	ft_unset_error(char *var, t_minishell *minishell);

#endif
