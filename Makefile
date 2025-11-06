# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kskender <kskender@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/04 00:00:00 by auto              #+#    #+#              #
#    Updated: 2025/11/06 17:35:02 by kskender         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = gcc
CFLAGS = -Wall -Wextra -Werror
READLINE_FLAGS = -lreadline

# Include directories
INCLUDES = -Iinclude -Iinclude/libft -Isrc/execution/executor_part -IGarbage_Collector

# Main source file (you need to create this)
MAIN_SRC = main.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)

# Globals (single definition of g_shell)
GLOBAL_SRC = globals.c
GLOBAL_OBJ = $(GLOBAL_SRC:.c=.o)

# Parser sources
PARSER_SRCS = \
	src/parsing/lexing/token_check.c \
	src/parsing/lexing/token_utils.c \
	src/parsing/lexing/tokenize.c \
	src/parsing/lexing/tokenize_helper.c \
	src/parsing/lexing/init.c \
	src/parsing/command/token_to_cmd.c \
	src/parsing/command/find_token.c \
	src/parsing/command/add_env.c \
	src/parsing/command/token_to_cmd_helper.c \
	src/parsing/command/expand_env.c \
	src/parsing/command/cmdlst_filelst.c

PARSER_OBJS = $(PARSER_SRCS:.c=.o)

# Executor sources
EXECUTOR_DIR = src/execution/executor_part
EXECUTOR_SRCS = \
	$(EXECUTOR_DIR)/exec_basics.c \
	$(EXECUTOR_DIR)/exec_basics1.c \
	$(EXECUTOR_DIR)/exec_builtins.c \
	$(EXECUTOR_DIR)/exec_builtins1.c \
	$(EXECUTOR_DIR)/exec_builtins2.c \
	$(EXECUTOR_DIR)/exec_builtins3.c \
	$(EXECUTOR_DIR)/exec_echo.c \
	$(EXECUTOR_DIR)/exec_external_handler.c \
	$(EXECUTOR_DIR)/exec_external_helpers.c \
	$(EXECUTOR_DIR)/exec_path.c \
	$(EXECUTOR_DIR)/exec_error.c \
	$(EXECUTOR_DIR)/exec_parent_runner.c \
	$(EXECUTOR_DIR)/exec_args_helpers.c \
	$(EXECUTOR_DIR)/exec_pipeline_helpers.c \
	$(EXECUTOR_DIR)/exec_redir_helpers.c \
	$(EXECUTOR_DIR)/exec_redir_attached.c \
	$(EXECUTOR_DIR)/exec_redir_separated.c \
	$(EXECUTOR_DIR)/exec_redir_heredoc.c \
	$(EXECUTOR_DIR)/exec_redir_infile.c \
	$(EXECUTOR_DIR)/exec_redir_outfile.c \
	$(EXECUTOR_DIR)/exec_redirections.c \
	$(EXECUTOR_DIR)/exec_utility_to_run.c \
	$(EXECUTOR_DIR)/exec_redirections1.c \
	$(EXECUTOR_DIR)/signals.c \
	$(EXECUTOR_DIR)/init_shell.c


EXECUTOR_OBJS = $(EXECUTOR_SRCS:.c=.o)

# Execution archive produced by executor part
LIBEXEC = $(EXECUTOR_DIR)/libexec.a

# Executor archive rule: build all executor and GC objs and archive them

$(LIBEXEC): $(EXECUTOR_OBJS) $(GC_OBJS)
	@echo "$(YELLOW)Building executor archive...$(RESET)"
	# Compile any missing Garbage_Collector object files
	@for src in $(GC_SRCS); do \
		obj=$${src%.c}.o; \
		if [ ! -f $$obj ]; then \
			echo "$(YELLOW)Compiling $$src...$(RESET)"; \
			$(CC) $(CFLAGS) $(INCLUDES) -c $$src -o $$obj || exit 1; \
		fi; \
	done
	# Compile any missing executor object files
	@for src in $(EXECUTOR_SRCS); do \
		obj=$${src%.c}.o; \
		if [ ! -f $$obj ]; then \
			echo "$(YELLOW)Compiling $$src...$(RESET)"; \
			$(CC) $(CFLAGS) $(INCLUDES) -c $$src -o $$obj || exit 1; \
		fi; \
	done
	ar rcs $(LIBEXEC) $(EXECUTOR_OBJS) $(GC_OBJS)

# Pattern rules to compile executor and GC sources to object files
$(EXECUTOR_DIR)/%.o: $(EXECUTOR_DIR)/%.c
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

Garbage_Collector/%.o: Garbage_Collector/%.c
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Garbage Collector sources
GC_DIR = Garbage_Collector
GC_SRCS = \
	$(GC_DIR)/garbage_collector.c \
	$(GC_DIR)/garbage_collector1.c \
	$(GC_DIR)/garbage_collector_utils.c \
	$(GC_DIR)/garbage_collector_utils1.c \
	$(GC_DIR)/garbage_collector_parser.c

GC_OBJS = $(GC_SRCS:.c=.o)

# Libft (adjust path as needed)
LIBFT_DIR = include/libft
LIBFT = $(LIBFT_DIR)/libft.a

# All objects (executor objects and GC objects are provided by LIBEXEC)
OBJS = $(MAIN_OBJ) $(GLOBAL_OBJ) $(PARSER_OBJS)

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(LIBFT) $(LIBEXEC) $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(RESET)"
	$(CC) $(CFLAGS) $(OBJS) $(LIBEXEC) $(LIBFT) $(READLINE_FLAGS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(RESET)"

$(LIBFT):
	@echo "$(YELLOW)Building libft...$(RESET)"
	$(MAKE) -C $(LIBFT_DIR)

%.o: %.c
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	rm -f $(OBJS)
	rm -f $(EXECUTOR_OBJS) $(GC_OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(YELLOW)Full clean...$(RESET)"
	rm -f $(NAME)
	rm -f $(LIBEXEC)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean
	$(MAKE) all

# Debug target
debug: CFLAGS += -g
debug: re

# Sanitize target
sanitize: CFLAGS += -fsanitize=address -g
sanitize: re

.PHONY: all clean fclean re debug sanitize