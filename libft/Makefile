SRC 		= ft_atoi.c ft_bzero.c ft_calloc.c ft_isalnum.c ft_isalpha.c ft_isascii.c ft_isdigit.c \
			  ft_isprint.c ft_itoa.c ft_memchr.c ft_memcmp.c ft_memcpy.c ft_memmove.c ft_memset.c \
			  ft_putchar_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_putstr_fd.c ft_split.c ft_strchr.c \
			  ft_strdup.c ft_striteri.c ft_strjoin.c ft_strlcat.c ft_strlcpy.c ft_strlen.c ft_strmapi.c \
			  ft_strncmp.c ft_strnstr.c ft_strrchr.c ft_strtrim.c ft_substr.c ft_tolower.c ft_toupper.c \
			  ft_lstnew.c ft_lstadd_front.c ft_lstsize.c ft_lstadd_back.c ft_lstdelone.c ft_lstlast.c \
			  ft_lstclear.c ft_lstiter.c ft_lstmap.c ft_pointer.c ft_print_hexa.c ft_fdprintf.c \
			  ft_printf.c ft_putchar.c ft_putnbr_unsigned.c ft_putnbr.c ft_putstr.c \
			  get_next_line.c ft_strcmp.c ft_free_arrays.c ft_strcmp_trim.c \


SRCS 		= ${addprefix ${PREROOT}, ${SRC}}

OBJS		= ${SRC:.c=.o}

PREROOT		= ./

HEAD 		= libft.h

NAME 		= libft.a

AR			= ar rc

RM			= rm -f

LIB			= ranlib

GCC			= cc

CFLAGS		= -Wall -Wextra -Werror

all:		${NAME}

make:		make all

.c.o:
			${GCC} ${CFLAGS} -c -I. $< -o $@



${NAME}:	${OBJS}
					${AR} ${NAME} ${OBJS}
					${LIB} ${NAME}


clean:
					rm -f ${OBJS}

fclean:clean
					rm -f ${addprefix ${PREROOT}, ${NAME}}

re:					fclean  all

.PHONY:				all clean fclean re
