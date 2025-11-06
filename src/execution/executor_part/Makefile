# Makefile for execution functions
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I../../../include -I../../../include/libft
OBJ_DIR = obj
NAME = libexec.a
SRC = \
	exec_basics.c \
	exec_basics1.c \
	exec_builtins.c \
	exec_builtins1.c \
	exec_builtins2.c \
	exec_builtins3.c \
	exec_echo.c \
	exec_external_handler.c \
	exec_external_helpers.c \
	exec_path.c \
	exec_error.c \
	exec_parent_runner.c \
	exec_args_helpers.c \
	exec_pipeline_helpers.c \
	exec_redir_helpers.c \
	exec_redir_attached.c \
	exec_redir_separated.c \
	exec_redir_heredoc.c \
	exec_redir_infile.c \
	exec_redir_outfile.c \
	exec_redirections.c \
	exec_utility_to_run.c \
		exec_redirections1.c \
		../../../Garbage_Collector/garbage_collector.c \
	../../../Garbage_Collector/garbage_collector1.c \
	../../../Garbage_Collector/garbage_collector_utils.c \
	../../../Garbage_Collector/garbage_collector_utils1.c \
	../../../Garbage_Collector/garbage_collector_parser.c \
	signals.c \
	init_shell.c

SRCS_BN := $(notdir $(SRC))
OBJS := $(addprefix $(OBJ_DIR)/,$(SRCS_BN:.c=.o))

all: $(NAME)

LIBFT = ../../../include/libft/libft.a
LIBFT_DIR = ../../../include/libft

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

# compile each source into $(OBJ_DIR)/<basename>.o
define MAKE_OBJ_RULE
$(OBJ_DIR)/$(patsubst %.c,%.o,$(notdir $(1))): $(1)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(1) -o $$@
endef

$(foreach src,$(SRC),$(eval $(call MAKE_OBJ_RULE,$(src))))


clean:
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
