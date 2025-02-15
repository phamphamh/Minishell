/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcousin <tcousin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by jspitz            #+#    #+#             */
/*   Updated: 2025/02/15 13:35:34 by tcousin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H

# define HEADER_H

# include "../libft/libft.h"
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

// Déclaration des fonctions de readline
void	rl_clear_history(void);
void	rl_redisplay(void);
int		rl_on_new_line(void);

// Types de tokens
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_EOF
} t_token_type;

// Structure pour les tokens
typedef struct s_token
{
	char			*value;
	int				type;
	struct s_token	*next;
	struct s_token	*prev;
} t_token;

// Structure pour les redirections
typedef struct s_redirection
{
	int						type;
	char					*file;
	struct s_redirection	*next;
} t_redirection;

// Structure pour les commandes
typedef struct s_cmd
{
	char				*name;
	char				**args;
	t_redirection		*redirs;
	int					pipe_in;
	int					pipe_out;
	struct s_cmd		*next;
} t_cmd;

// Structure en liste chaînée pour le garbage collector
typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
} t_gc_node;

// Structure pour les variables d'environnement
typedef struct s_env
{
	char			*var;
	struct s_env	*next;
} t_env;

// Structure principale du minishell
typedef struct s_minishell
{
	t_env		*env;
	t_gc_node	*gc_head;
	t_token		*tokens;
	t_cmd		*commands;
	int			exit_nb;
} t_minishell;

// Prototypes des fonctions
// main.c
void	ft_initialize(t_minishell *minishell, char **envp);
t_env	*ft_env_to_list(char **envp, t_minishell *minishell);

// garbage_collector.c
bool	ft_gc_add(t_gc_node **gc_head, void *ptr);
bool	ft_gc_remove(t_gc_node **gc_head, void *ptr);
void	ft_gc_clear(t_gc_node **gc_head);
void	ft_gc_remove_list(t_gc_node **gc_head, t_token *tokens);

// signal_handler.c
void	ft_setup_signals(void);
void	ft_reset_signals(void);
void	ft_ignore_signals(void);

// builtins.c
int		ft_execute_builtin(t_cmd *cmd, t_minishell *minishell);
void	ft_print_env(t_env *env);
char	**ft_env_to_array(t_env *env);
bool	ft_is_builtin(char *value);

// builtins_utils.c
void	ft_handle_export_var(t_minishell *minishell, char *var);
void	ft_handle_unset_var(t_minishell *minishell, char *var_name);
void	ft_print_export_list(t_env *env);
void	ft_print_export_var(t_env *env_var);
void	update_env_var(t_env *env, const char *name, const char *new_value, t_minishell *minishell);
void	update_pwd_and_oldpwd(t_minishell *minishell);

// exec.c
void	ft_execute_command(t_cmd *cmd, t_minishell *minishell);
char	*ft_find_executable(char *cmd_name, t_env *env);

// pipe_handler.c
int		ft_create_pipe(t_cmd *cmd);
void	ft_close_pipes(t_cmd *cmd);
void	ft_setup_pipes(t_cmd *cmd);

// redirection.c
int		ft_handle_redirection(t_redirection *redir);
void	ft_restore_fds(int saved_stdin, int saved_stdout);

// tokenizer.c
int ft_is_operator(char c);
char *ft_allocate_expanded(char *input);
void ft_fill_expanded(char *input, char *expanded);
char *ft_expand_operators(char *input);
int ft_determine_token_type(char *token);
t_token	*ft_tokenize(char *input, t_minishell *minishell);
//int		ft_determine_token_type(char *token);

// expand env variables
char *expand_env_vars(const char *str, t_minishell *minishell);

// split_with_quotes.c
char	**ft_split_with_quotes(const char *s, char delimiter, t_minishell *minishell);
bool	is_quote(char c);
char	*allocate_token(const char *start, int token_len, char **tokens, int token_count);
int		skip_quotes(const char *s, int i, char *quote);

// split_utils.c
int		handle_token(const char *s, int i, char delimiter, char **tokens, int *token_count, t_minishell *minishell);

// cmd_parser.c
int 	ft_count_args(t_token *start);
t_redirection *ft_create_redirection(t_token *token, t_token *next, t_minishell *minishell);
void 	ft_process_redirection(t_token **current, t_cmd *cmd, t_minishell *minishell);
int		ft_check_syntax_errors(t_token *tokens);
t_cmd	*tokens_to_cmds(t_token *tokens, t_minishell *minishell);

// utils.c
void	ft_clean_exit(t_minishell *minishell, int exit_num);
bool	ft_is_valid_identifier(const char *str);
bool	ft_is_valid_identifier_before_equal(const char *str);
int		ft_env_var_match(const char *env_var, const char *var_name);
t_env	*ft_find_env_var(t_env *env, const char *var);
void	ft_add_env_var(t_minishell *minishell, const char *var);
bool	ft_is_all_nb(char *str);

#endif
