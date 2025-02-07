/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by jspitz            #+#    #+#             */
/*   Updated: 2025/02/07 12:32:11 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H

# define HEADER_H

# include "../libft/libft.h"
# include "exec.h"
# include "parsing.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/param.h>
# include <signal.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <curses.h>
# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <limits.h>

/*Je vais commencer par tout mettre dans ce .h
jusqu'a qu'on ne s'y retrouve plus
*/

// Structure en liste chainée pour le garbage collector.
typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
}	t_gc_node;

// Structure pour les variables d'environnement.
typedef struct s_env
{
	char			*var;
	struct s_env	*next;
}				t_env;

typedef struct s_minishell
{
	t_env		*env;
	t_gc_node	*gc_head;
	t_token		*tokens; // legacy
	t_cmd		*commands;
	int			exit_nb;
}	t_minishell;

// main.c
t_env	*ft_env_to_list(char **envp, t_minishell *minishell);
void	ft_initialize(char **envp, t_minishell *minishell);

// garbage_collector.c
bool	ft_gc_add(t_gc_node **gc_head, void *ptr);
bool	ft_gc_remove(t_gc_node **gc_head, void *ptr);
void	ft_gc_clear(t_gc_node **gc_head);
void	ft_gc_remove_list(t_gc_node **gc_head, t_token *tokens);

/* Fonctions de validation pour export/unset */
int		ft_is_valid_identifier(const char *str);
int		ft_is_valid_identifier_before_equal(const char *str);

/* Fonctions de gestion des variables d'environnement */
int		ft_env_var_match(const char *env_var, const char *var_name);
t_env	*ft_find_env_var(t_env *env, const char *var);
void	ft_add_env_var(t_minishell *minishell, const char *var);
int		ft_env_list_size(t_env *env);
t_env	**ft_sort_env_list(t_env *env, int size);
void	ft_bubble_sort_env(t_env **arr, int size);
void	ft_print_export_var(t_env *env_var);
void	ft_print_export_list(t_env *env);
void	ft_handle_export_var(t_minishell *minishell, char *var);
void	ft_handle_unset_var(t_minishell *minishell, char *var_name);
void	ft_export_error(char *var, t_minishell *minishell);
void	ft_unset_error(char *var, t_minishell *minishell);

#endif
