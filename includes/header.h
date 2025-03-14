/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:34:29 by jspitz            #+#    #+#             */
/*   Updated: 2025/03/14 14:03:15 by yboumanz         ###   ########.fr       */
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
	bool					has_pipe;
	struct s_cmd			*next;
	struct s_cmd			*prev;
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
	bool					env_cleaned;
}							t_minishell;

// structure pour faciliter l'expand d'env sans devoir passer 5 argument
typedef struct s_expand_env
{
	char					*res;
	int						*j;
	int						buf_size;
	t_minishell				*ms;
}							t_expand_env;

typedef struct s_split_env
{
	char					**tokens;
	int						token_count;
	char					delimiter;
	char					quote;
	bool					is_heredoc;
	t_minishell				*ms;
}							t_split_env;

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

typedef struct s_redir_info
{
	t_redirection			*last_in;
	t_redirection			*last_out;
	t_redirection			*last_heredoc;
	int						stdin_fd;
	t_minishell				*minishell;
}							t_redir_info;

// Prototypes des fonctions

// main.c
void						check_env_initialization(t_minishell *minishell,
								char **envp);
void						ft_initialize(t_minishell *minishell, char **envp);
t_env						*ft_env_to_list(char **envp,
								t_minishell *minishell);
void						ft_execute_builtin_command(t_cmd *cmd,
								t_minishell *minishell);
void						ft_execute_child_process(t_cmd *cmd,
								t_minishell *minishell);
pid_t						ft_fork_and_execute(t_cmd *cmd,
								t_minishell *minishell);
void						ft_wait_child(pid_t pid, int *status,
								t_minishell *minishell, int last);
void						ft_handle_child_signal(int status,
								t_minishell *minishell, int last_cmd);
int							ft_wait_child_for_pid(t_minishell *minishell,
								pid_t pid);
void						ft_process_line(char *line, t_minishell *minishell);
int							ft_check_process_status(pid_t *pids, int cmd_count,
								t_minishell *minishell, int *remaining);
void						wait_for_processes(pid_t *pids, int cmd_count,
								t_minishell *minishell);

// garbage_collector.c
void						free_env_list(t_minishell *minishell);
void						free_gc_list(t_minishell *minishell);
void						ft_clean_exit(t_minishell *minishell, int exit_num);
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
bool						ft_is_builtin(char *value);
void						ft_print_export_list(t_env *env);
void						update_env_var(t_env *env, const char *name,
								const char *new_value, t_minishell *minishell);
void						update_pwd_and_oldpwd(t_minishell *minishell);

// builtins_utils2.c
void						ft_print_export_var(t_env *env_var);

// builtins_utils3.c
void						ft_remove_env_var(t_minishell *minishell,
								t_env *to_remove);
void						update_env_var(t_env *env, const char *name,
								const char *new_value, t_minishell *minishell);
int							ft_validate_export_var(t_minishell *minishell,
								char *var, char **var_name, char **equal_pos);
int							ft_handle_export_var(t_minishell *minishell,
								char **var);
int							ft_handle_unset_var(t_minishell *minishell,
								char **var_name);
void						ft_export_error(char *var, t_minishell *minishell);
void						ft_unset_error(char *var, t_minishell *minishell);

// builtins_cd.c
int							ft_cd(t_cmd *cmd, t_minishell *minishell);

// builtins_echo.c
int							ft_echo(t_cmd *cmd);

// builtins_pwd.c
int							ft_pwd(t_minishell *minishell);

// builtins_exit.c
int							ft_exit(t_cmd *cmd, t_minishell *minishell);

// exec.c
void						close_pipe_fds(t_cmd *cmd);
void						ft_close_all_pipes(t_cmd *cmd_first);
char						**ft_allocate_paths(char *path_value);
void						free_paths(char **paths, int i);
char						**ft_get_paths(t_env *env);
char						*ft_find_cmd_in_paths(char **paths, char *cmd_name);
char						*ft_find_executable(char *cmd_name, t_env *env);
void						ft_close_unused_fds(t_cmd *cmd);
void						handle_cmd_not_found(t_cmd *cmd,
								t_minishell *minishell);
void						handle_is_directory(char *cmd_path,
								t_minishell *minishell);
void						handle_permission_denied(char *cmd_path,
								t_minishell *minishell);
void						handle_memory_error(char *cmd_path,
								t_minishell *minishell);
void						handle_execve_failure(char *cmd_path,
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
void						ft_exec_heredoc_child(t_cmd *cmd,
								t_redirection *output_redir,
								t_minishell *minishell, int heredoc_fd);
void						ft_heredoc_child_exit(t_minishell *minishell,
								int exit_code, int fd_to_close);
int							ft_wait_heredoc(pid_t pid, int heredoc_pipe[2]);
t_redirection				*ft_get_output_redir(t_redirection *redir);
int							ft_apply_redirections(t_cmd *cmd,
								t_redir_info *struct_redir);
int							ft_setup_input_redirection(t_cmd *cmd,
								t_redir_info *redir);
int							ft_setup_output_redirection(t_cmd *cmd,
								t_redir_info *redir);

void						ft_update_last_redirections(t_redirection *current,
								t_redirection **last_out,
								t_redirection **last_in,
								t_redirection **last_heredoc);
int							ft_read_heredoc(t_redirection *last_heredoc,
								int pipe_fd);
void						ft_setup_heredoc_child(int heredoc_fd,
								t_redirection *output_redir);
int							ft_handle_heredoc(t_redirection *last_heredoc,
								t_cmd *heredoc_cmd, t_minishell *minishell);
void						ft_find_last_redirections(t_redirection *redir,
								t_redirection **last_out,
								t_redirection **last_in,
								t_redirection **last_heredoc);
int							ft_handle_redirection(t_cmd *cmd,
								t_redirection *redir, bool restore_after,
								t_minishell *minishell);
void						ft_restore_fds(int saved_stdin, int saved_stdout);
void						ft_save_fds(int *saved_stdin, int *saved_stdout);

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
								t_minishell *minishell, bool in_quotes,
								bool is_heredoc);

// split_with_quotes.c
char						**ft_split_with_quotes(const char *s,
								char delimiter, t_minishell *minishell,
								bool is_heredoc);
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
void						set_default_path(t_minishell *minishell);
void						ft_clean_exit(t_minishell *minishell, int exit_num);
void						ft_clean_env_list(t_env *env);
bool						ft_is_valid_identifier(const char *str);
int							ft_env_var_match(const char *env_var,
								const char *var_name);
t_env						*ft_find_env_var(t_env *env, const char *var);
void						ft_add_env_var(t_minishell *minishell,
								const char *var);
bool						ft_is_all_nb(char *str);

void						ft_foreach_cmd(t_cmd *cmd, t_minishell *minishell,
								pid_t *last_pid, pid_t *pids);

#endif
