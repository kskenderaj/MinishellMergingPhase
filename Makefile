# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/04 00:00:00 by auto              #+#    #+#              #
#    Updated: 2025/11/28 15:44:30 by jtoumani         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.DEFAULT_GOAL := all

NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
READLINE_FLAGS = -lreadline -lncurses

# Include directories
INCLUDES = -Iinclude -Iinclude/libft -Isrc/execution/executor_part -IGarbage_Collector -Imain

# Main directory sources
MAIN_SRCS = \
	main/main.c \
	main/main_loop.c \
	main/input_processing.c \
	main/env_utils.c \
	main/env_merge.c \
	main/single_cmd_setup.c \
	main/single_cmd_env.c \
	main/single_cmd.c \
	main/pipeline.c

MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Parser sources
PARSER_SRCS = \
	src/parsing/lexing/token_check.c \
	src/parsing/lexing/token_utils.c \
	src/parsing/lexing/tokenize.c \
	src/parsing/lexing/tokenize_helper.c \
	src/parsing/lexing/tokenize_helper2.c \
	src/parsing/lexing/tokenize_helper3.c \
	src/parsing/lexing/init.c \
	src/parsing/command/token_to_cmd.c \
	src/parsing/command/find_token.c \
	src/parsing/command/add_env.c \
	src/parsing/command/add_env2.c \
	src/parsing/command/token_to_cmd_helper.c \
	src/parsing/command/token_to_cmd_helper2.c \
	src/parsing/command/token_to_cmd_helper3.c \
	src/parsing/command/token_to_cmd_helper4.c \
	src/parsing/command/expand_env.c \
	src/parsing/command/expand_env2.c \
	src/parsing/command/cmdlst_filelst.c \
	src/parsing/command/cmdlst_filelst2.c \
	src/parsing/command/field_split.c \
	src/parsing/command/ifs_split.c \
	src/parsing/command/heredoc_utils.c \
	src/parsing/command/read_heredoc.c \
	src/parsing/command/read_heredoc_utils.c 
	
PARSER_OBJS = $(PARSER_SRCS:.c=.o)

# Executor sources
EXECUTOR_DIR = src/execution/executor_part
EXECUTOR_SRCS = \
	$(EXECUTOR_DIR)/exec_builtins.c \
	$(EXECUTOR_DIR)/exec_builtins1.c \
	$(EXECUTOR_DIR)/exec_builtins2.c \
	$(EXECUTOR_DIR)/exec_builtins3.c \
	$(EXECUTOR_DIR)/exec_builtins4.c \
	$(EXECUTOR_DIR)/exec_builtins5.c \
	$(EXECUTOR_DIR)/exec_echo.c \
	$(EXECUTOR_DIR)/exec_external_handler.c \
	$(EXECUTOR_DIR)/exec_external_handler1.c \
	$(EXECUTOR_DIR)/exec_external_handler2.c \
	$(EXECUTOR_DIR)/exec_external_handler3.c \
	$(EXECUTOR_DIR)/exec_external_helpers.c \
	$(EXECUTOR_DIR)/exec_path.c \
	$(EXECUTOR_DIR)/exec_args_helpers.c \
	$(EXECUTOR_DIR)/exec_args_helpers2.c \
	$(EXECUTOR_DIR)/exec_args_helpers3.c \
	$(EXECUTOR_DIR)/exec_pipeline_helpers.c \
	$(EXECUTOR_DIR)/exec_pipeline_helpers1.c \
	$(EXECUTOR_DIR)/exec_pipeline_helpers2.c \
	$(EXECUTOR_DIR)/exec_pipeline_helpers3.c \
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
	$(EXECUTOR_DIR)/signals2.c \
	$(EXECUTOR_DIR)/init_shell.c

EXECUTOR_OBJS = $(EXECUTOR_SRCS:.c=.o)

# Garbage Collector sources
GC_DIR = Garbage_Collector
GC_SRCS = \
	$(GC_DIR)/garbage_collector.c \
	$(GC_DIR)/garbage_collector1.c \
	$(GC_DIR)/garbage_collector_utils.c \
	$(GC_DIR)/garbage_collector_utils1.c \
	$(GC_DIR)/garbage_collector_parser.c

GC_OBJS = $(GC_SRCS:.c=.o)

# Execution archive
LIBEXEC = $(EXECUTOR_DIR)/libexec.a

# Libft
LIBFT_DIR = include/libft
LIBFT = $(LIBFT_DIR)/libft.a

# All objects
OBJS = $(MAIN_OBJS) $(PARSER_OBJS)

# Colors
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
RESET = \033[0m

# Silent mode if not verbose
ifndef VERBOSE
.SILENT:
endif

all: $(NAME)

$(NAME): $(LIBFT) $(LIBEXEC) $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(RESET)"
	$(CC) $(CFLAGS) $(OBJS) $(LIBEXEC) $(LIBFT) $(READLINE_FLAGS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(RESET)"

$(LIBEXEC): $(EXECUTOR_OBJS) $(GC_OBJS)
	@echo "$(YELLOW)Building executor archive...$(RESET)"
	ar rcs $(LIBEXEC) $(EXECUTOR_OBJS) $(GC_OBJS)

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

re: fclean all

debug: CFLAGS += -g
debug: re

sanitize: CFLAGS += -fsanitize=address -g
sanitize: re

.PHONY: all clean fclean re debug sanitize