/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by jspitz            #+#    #+#             */
/*   Updated: 2025/03/11 17:27:42 by yboumanz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H

# define HEADER_H

# include "../libft/libft.h"
# include <curses.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <sys/param.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

// Variable globale pour gérer les signaux
extern int					g_signal_received;

// Déclaration des fonctions de readline
void						rl_clear_history(void);
void						rl_redisplay(void);
int							rl_on_new_line(void);
void						rl_replace_line(const char *text, int clear_undo);

// Types de tokens
typedef enum e_token_type
{
	TOKEN_CMD,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_EOF
}							t_token_type;

// Structure pour les tokens
typedef struct s_token
{
	char					*value;
	int						type;
	struct s_token			*next;
	struct s_token			*prev;
}							t_token;

// Structure pour les redirections
typedef struct s_redirection
{
	int						type;
	char					*file;
	struct s_redirection	*next;
}							t_redirection;

// Structure pour les commandes
typedef struct s_cmd
{
	char					*name;
	char					**args;
	t_redirection			*redirs;
	int						pipe_in;
	int						pipe_out;
	struct s_cmd			*next;
}							t_cmd;

// Structure en liste chaînée pour le garbage collector
typedef struct s_gc_node
{
	void					*ptr;
	struct s_gc_node		*next;
}							t_gc_node;

// Structure pour les variables d'environnement
typedef struct s_env
{
	char					*var;
	struct s_env			*next;
}							t_env;

// Structure principale du minishell
typedef struct s_minishell
{
	t_env					*env;
	t_gc_node				*gc_head;
	t_token					*tokens;
	t_cmd					*commands;
	int						exit_nb;
}							t_minishell;

// structure pour faciliter l'expand d'env sans devoir passer 5 argument
typedef struct s_expand_env
{
	char					*res;
	int						*j;
	int						buf_size;
	t_minishell				*ms;
}							t_expand_env;

// structure pour faciliter le split sans devoir passer 5 argument
// (fuck la norme)
typedef struct s_split_env
{
	char					**tokens;
	int						token_count;
	char					delimiter;
	char					quote;
	t_minishell				*ms;
}							t_split_env;

// structure pour faciliter l'expand pour les characteres speciaux sans
// devoir passer 5 argument
typedef struct s_expand_state
{
	char					*input;
	char					*expanded;
	int						*i;
	int						*j;
}							t_expand_state;

// structure pour faciliter la tokenisation sans devoir passer 5 argument
typedef struct s_tokenizer
{
	t_token					*prev;
	t_token					*token_list;
	t_minishell				*minishell;
	int						is_cmd;
	int						skip_concat;
}							t_tokenizer;

// Prototypes des fonctions
// main.c
void						ft_initialize(t_minishell *minishell, char **envp);
t_env						*ft_env_to_list(char **envp,
								t_minishell *minishell);

// garbage_collector.c
bool						ft_gc_add(t_gc_node **gc_head, void *ptr);
bool						ft_gc_remove(t_gc_node **gc_head, void *ptr);
void						ft_gc_clear(t_gc_node **gc_head);
void						ft_gc_remove_list(t_gc_node **gc_head,
								t_token *tokens);
void						ft_gc_remove_cmds(t_gc_node **gc_head, t_cmd *cmds);
void						ft_gc_remove_env(t_gc_node **gc_head, t_env *env);

// signal_handler.c
void						ft_setup_signals(void);
void						ft_reset_signals(void);
void						ft_ignore_signals(void);
void						ft_heredoc_signals(void);

// builtins.c
int							ft_execute_builtin(t_cmd *cmd,
								t_minishell *minishell);
void						ft_print_env(t_env *env);
char						**ft_env_to_array(t_minishell *minishell,
								t_env *env);
bool						ft_is_builtin(char *value);

// builtins_utils.c
void						ft_handle_export_var(t_minishell *minishell,
								char *var);
void						ft_handle_unset_var(t_minishell *minishell,
								char *var_name);
void						ft_print_export_list(t_env *env);
void						ft_print_export_var(t_env *env_var);
void						update_env_var(t_env *env, const char *name,
								const char *new_value, t_minishell *minishell);
void						update_pwd_and_oldpwd(t_minishell *minishell);
void						ft_export_error(char *var, t_minishell *minishell);
void						ft_unset_error(char *var, t_minishell *minishell);

// exec.c
void						ft_execute_command(t_cmd *cmd,
								t_minishell *minishell);
char						*ft_find_executable(char *cmd_name, t_env *env);
void						ft_execute_child(t_cmd *cmd,
								t_minishell *minishell);
void						ft_close_unused_fds(t_cmd *cmd);

// pipe_handler.c
int							ft_create_pipe(t_cmd *cmd);
void						ft_close_pipes(t_cmd *cmd);
void						ft_setup_pipes(t_cmd *cmd);
void						ft_close_all_pipes(t_cmd *cmd_first);

// redirection.c
int							ft_handle_redirection(t_cmd *cmd,
								t_redirection *redir);
void						ft_restore_fds(int saved_stdin, int saved_stdout);

// tokenizer.c
int							ft_is_operator(char c);
int							ft_handle_space_token(char *token);
void						ft_concat_tokens(t_token *prev, char *token,
								t_minishell *minishell);
char						*ft_allocate_expanded(char *input);
char						*ft_remove_escape_chars(char *str);
void						ft_process_character(t_expand_state *state,
								char *in_quotes);
t_token						*ft_create_token(char *value, int type,
								t_minishell *minishell);
t_token						*ft_add_token(t_token **token_list,
								t_token *new_token);
int							ft_process_token(char **split_input,
								t_tokenizer *t);
char						*ft_expand_operators(char *input);
int							ft_determine_token_type(char *token, int *is_cmd,
								t_token *prev);
t_token						*ft_tokenize(char *input, t_minishell *minishell);
// int		ft_determine_token_type(char *token);

// expand env variables
int							extract_var_name(const char *str, int i,
								char **var_name);
void						process_escape_sequence(const char *str, char *res,
								int *i, int *j);
char						*prepare_result_buffer(const char *str,
								t_minishell *ms);
char						*expand_env_vars(const char *str,
								t_minishell *minishell, bool in_quotes);

// split_with_quotes.c
char						**ft_split_with_quotes(const char *s,
								char delimiter, t_minishell *minishell);
bool						is_quote(char c);
char						*allocate_token(const char *start, int token_len,
								char **tokens, int token_count);
int							skip_quotes(const char *s, int i, char *quote);

// split_utils.c
int							handle_token(const char *s, int i,
								t_split_env *env);

// cmd_parser.c
void						ft_set_command_name(t_cmd *cmd,
								t_minishell *minishell);
int							ft_count_args(t_token *start);
t_redirection				*ft_create_redirection(t_token *token,
								t_token *next, t_minishell *minishell);
void						ft_process_redirection(t_token **current,
								t_cmd *cmd, t_minishell *minishell);
t_cmd						*tokens_to_cmds(t_token *tokens,
								t_minishell *minishell);

// check syntax
int							ft_special_token_msg(char *value);
int							ft_check_special_token(char *value);
int							ft_check_syntax_errors(t_token *tokens);

// utils.c
void						ft_clean_exit(t_minishell *minishell, int exit_num);
void						ft_clean_env_list(t_env *env);
bool						ft_is_valid_identifier(const char *str);
bool						ft_is_valid_identifier_before_equal(const char *str);
int							ft_env_var_match(const char *env_var,
								const char *var_name);
t_env						*ft_find_env_var(t_env *env, const char *var);
void						ft_add_env_var(t_minishell *minishell,
								const char *var);
bool						ft_is_all_nb(char *str);

#endif
