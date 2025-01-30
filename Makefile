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
CFLAGS = -Wall -Wextra -Werror -g
RM = rm -f

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

SRC_DIR = srcs

SRCS =	$(SRC_DIR)/main.c $(SRC_DIR)/garbage_collector.c $(SRC_DIR)/parsing/tokenizer.c \
		$(SRC_DIR)/exec/exec.c $(SRC_DIR)/builtins/builtins.c $(SRC_DIR)/builtins/builtins_utils.c \
		$(SRC_DIR)/parsing/parsing.c $(SRC_DIR)/parsing/cmd_utils.c $(SRC_DIR)/parsing/redir_utils.c \
		$(SRC_DIR)/parsing/split_utils.c $(SRC_DIR)/parsing/split_with_quotes.c $(SRC_DIR)/parsing/tokenizer_utils.c \
		$(SRC_DIR)/parsing/expand.c \

OBJS = $(SRCS:.c=.o)

INCLUDES = -I./includes -I./$(LIBFT_DIR)

LDFLAGS = -lreadline

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --trace-children=yes --track-fds=yes $(V_FLAG)
VALGRIND_OTHER = valgrind --vgdb=yes
V_FLAG = --suppressions="data/ignore_valgrind"
HELLGRIND = valgrind --tool=helgrind -g3

all: $(NAME)
	@echo "Build completed successfully."

$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(OBJS) -L$(LIBFT_DIR) -lft -o $(NAME) $(LDFLAGS)

$(LIBFT):
	@make -C $(LIBFT_DIR)

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
	done < data/test
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
	done < data/test
	@$(RM) output_minishell.txt output_bash.txt

valgrind: $(NAME)
	@$(VALGRIND) ./$(NAME)

valgrind_other: $(NAME)
	@$(VALGRIND_OTHER) ./$(NAME)

helgrind: $(NAME)
	@$(HELLGRIND) ./$(NAME)

.PHONY: all clean fclean re run runv valgrind valgrind_other helgrind
