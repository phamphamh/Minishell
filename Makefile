# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yboumanz <yboumanz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+           #
#    Created: 2025/01/14 13:04:12 by yboumanz          #+#    #+#              #
#    Updated: 2025/01/14 13:04:14 by yboumanz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

SRCS = srcs/main.c \
	   srcs/env_to_list.c \
	   srcs/ft_process_line.c \
	   srcs/ft_process_line_utils.c \
       srcs/garbage_collector.c \
       srcs/garbage_collector_utils.c \
       srcs/signal_handler.c \
       srcs/builtins/builtins_utils2.c \
       srcs/builtins/builtins_utils3.c \
       srcs/builtins/builtins_utils_part1.c \
       srcs/builtins/builtins_utils_part2.c \
       srcs/builtins/builtins_utils_part3.c \
       srcs/builtins/builtins_utils_common.c \
       srcs/builtins/builtins_cd.c \
       srcs/builtins/builtins_echo.c \
       srcs/builtins/builtins_pwd.c \
       srcs/builtins/builtins_exit.c \
       srcs/builtins/builtins_execute_builtin.c \
       srcs/exec/exec.c \
       srcs/exec/exec_utils.c \
       srcs/exec/exec_utils2.c \
       srcs/exec/exec_utils3.c \
       srcs/exec/ft_execute_process.c \
       srcs/exec/signal_handler_utils.c \
       srcs/exec/pipe_handler.c \
       srcs/exec/redirection.c \
       srcs/exec/heredoc.c \
       srcs/exec/heredoc_utils.c \
       srcs/exec/ft_read_heredoc.c \
       srcs/exec/redirection_utils.c \
       srcs/exec/redirection_utils2.c \
       srcs/exec/setup_redirection.c \
       srcs/parser/tokenizer.c \
       srcs/parser/tokenizer_utils.c \
       srcs/parser/tokenizer_utils2.c \
       srcs/parser/tokenizer_utils3.c \
       srcs/parser/process_tokens.c \
       srcs/parser/cmd_parser.c \
       srcs/parser/cmd_parser_utils.c \
       srcs/parser/parser_utils.c \
       srcs/parser/parser_utils2.c \
       srcs/parser/split_with_quotes.c \
       srcs/parser/split_utils.c \
       srcs/parser/check_syntax_utils.c \
       srcs/parser/check_syntax.c \
       srcs/parser/expand_env_utils.c \
       srcs/parser/expand_env.c \
       srcs/utils.c \
       srcs/utils2.c \

OBJS = $(SRCS:.c=.o)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

INCLUDES = -I./includes -I./$(LIBFT_DIR)

LIBS = -L$(LIBFT_DIR) -lft -L/usr/local/lib -lreadline

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --trace-children=yes --track-fds=yes $(V_FLAG)
VALGRIND_OTHER = valgrind --vgdb=yes
V_FLAG = --suppressions="valgrind_supp"
HELLGRIND = valgrind --tool=helgrind -g3

all: $(LIBFT) $(NAME)
	@echo "Build completed successfully."

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) $(OBJS)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@$(RM) $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

run: $(NAME)
	@echo "Running tests and comparing with Bash POSIX..."
	@while read -r line; do \
		echo "----------------------------------------"; \
		echo "Running test: $$line"; \
		echo "----------------------------------------"; \
		echo "$$line" | ./$(NAME) > output_minishell.txt; \
		echo "Minishell output:"; \
		cat output_minishell.txt; \
		echo "----------------------------------------"; \
		echo "$$line" | /bin/bash --posix > output_bash.txt; \
		echo "Bash POSIX output:"; \
		cat output_bash.txt; \
		echo "----------------------------------------"; \
	done < tests/test
	@$(RM) output_minishell.txt output_bash.txt

runv: $(NAME)
	@echo "Running tests with Valgrind and comparing with Bash POSIX..."
	@while read -r line; do \
		echo "----------------------------------------"; \
		echo "Running test: $$line"; \
		echo "----------------------------------------"; \
		echo "$$line" | ./$(NAME) > output_minishell.txt; \
		echo "Minishell output:"; \
		cat output_minishell.txt; \
		echo "----------------------------------------"; \
		echo "$$line" | /bin/bash --posix > output_bash.txt; \
		echo "Bash POSIX output:"; \
		cat output_bash.txt; \
		echo "----------------------------------------"; \
		echo "Running Valgrind..."; \
		echo "$$line" | $(VALGRIND) ./$(NAME); \
		echo "----------------------------------------"; \
	done < tests/test
	@$(RM) output_minishell.txt output_bash.txt

valgrind: $(NAME)
	@$(VALGRIND) ./$(NAME)

valgrind_other: $(NAME)
	@$(VALGRIND_OTHER) ./$(NAME)

helgrind: $(NAME)
	@$(HELLGRIND) ./$(NAME)

.PHONY: all clean fclean re run runv valgrind valgrind_other helgrind
