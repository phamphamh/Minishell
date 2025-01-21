#ifndef PARSING_H
# define PARSING_H

typedef struct s_minishell t_minishell;

typedef enum e_token_type
{
	CMD = -1,
	ARG = -2,
	PIPE = -3,
	REDIR_IN = -4,
	REDIR_OUT = -5,
	REDIR_APPEND = -6,
	HERE_DOC = -7,
	LIM = -8,
	FILENAME = -9,
}				t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}				t_token;

typedef struct s_redirection
{
    char                    *file;  // Nom du fichier
    int                     type;   // Type : REDIR_IN, REDIR_OUT, APPEND, HERE_DOC
    struct s_redirection    *next;  // Pointeur vers la prochaine redirection
} t_redirection;

typedef struct s_cmd
{
    char            **args;          // Tableau des arguments
    t_redirection   *input_redir;    // Liste chaînée des redirections d'entrée
    t_redirection   *output_redir;   // Liste chaînée des redirections de sortie
    int             is_pipeline;     // 1 si c'est une commande en pipeline
    struct s_cmd    *next;           // Pointeur vers la commande suivante
} t_cmd;


// tokenizer.c
bool	ft_integrate_token(char *value, int type, t_token **token_list, t_minishell *minishell);
t_token	*ft_tokenize(char *input, t_minishell *minishell);
int		ft_determine_token_type(char *token, char *prev_token);
t_token	*ft_create_token(char *value, int type, t_minishell *minishell);

// parser.c
void	ft_parse(char *input, t_minishell *minishell);
void 	print_commands(t_cmd *cmd_list);


t_cmd *init_cmd(t_minishell *minishell);
void add_arg_to_cmd(t_cmd *cmd, char *arg, t_minishell *minishell);


void add_redirection(t_redirection **redir_list, t_token *token, t_minishell *minishell);
#endif
