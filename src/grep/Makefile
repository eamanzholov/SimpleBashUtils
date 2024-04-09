NAME = s21_grep

CC = gcc
CFLAGS = 	-Wall -Wextra -Werror -std=c11
RM = rm -rf

SRCS=	$(wildcard *.c)
OBJS=	$(SRCS:.c=.o)


all: ${NAME}

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

${NAME}: ${OBJS}
	${CC} ${OBJS} ${CFLAGS} -o ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: clean fclean re all