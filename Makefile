# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/11 08:15:00 by klejdi            #+#    #+#              #
#    Updated: 2025/11/11 04:26:32 by klejdi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = gcc
CFLAGS = -Wall -Wextra -Werror
READLINE_FLAGS = -lreadline

# --- DIRECTORIES ---
OBJ_DIR = obj
LIBFT_DIR = include/libft
INCLUDE_DIRS = -Iinclude -I$(LIBFT_DIR)

# --- SOURCE FILES (MANUALLY LISTED FOR 42 NORM) ---
SRCS = \
    main.c \
    src/main/debug.c \
    globals.c \
    src/main/assigns.c \
    src/main/builtins.c \
    src/main/dispatch.c \
    src/main/env_utils.c \
    src/main/execute.c \
    src/main/exit.c \
    src/main/exit_code.c \
    src/main/expand.c \
    src/main/input.c \
    src/main/pipeline.c \
    src/parsing/command/add_env.c \
    src/parsing/command/cmdlst_filelst.c \
    src/parsing/command/expand_env.c \
    src/parsing/command/find_token.c \
    src/parsing/command/token_to_cmd_helper.c \
    src/parsing/lexing/init.c \
    src/parsing/lexing/token_check.c \
    src/parsing/lexing/token_utils.c \
    src/parsing/lexing/tokenize_helper.c \
    src/parsing/parser.c \
    src/parsing/tokenizer.c \
    src/execution/executor_part/exec_args_helpers.c \
    src/execution/executor_part/exec_basics.c \
    src/execution/executor_part/exec_basics1.c \
    src/execution/executor_part/exec_builtins.c \
    src/execution/executor_part/exec_builtins1.c \
    src/execution/executor_part/exec_builtins2.c \
    src/execution/executor_part/exec_builtins3.c \
    src/execution/executor_part/exec_echo.c \
    src/execution/executor_part/exec_error.c \
    src/execution/executor_part/exec_external_handler.c \
    src/execution/executor_part/exec_external_helpers.c \
    src/execution/executor_part/exec_main_utilities.c \
    src/execution/executor_part/exec_parent_runner.c \
    src/execution/executor_part/exec_path.c \
    src/execution/executor_part/exec_pipeline_helpers.c \
    src/execution/executor_part/exec_redir_attached.c \
    src/execution/executor_part/exec_redir_helpers.c \
    src/execution/executor_part/exec_redir_heredoc.c \
    src/execution/executor_part/exec_redir_infile.c \
    src/execution/executor_part/exec_redir_outfile.c \
    src/execution/executor_part/exec_redir_separated.c \
    src/execution/executor_part/exec_redirections.c \
    src/execution/executor_part/exec_redirections1.c \
    src/execution/executor_part/exec_utility_to_run.c \
    src/execution/executor_part/init_shell.c \
    src/execution/executor_part/signals.c \
    Garbage_Collector/free_structs.c \
    Garbage_Collector/garbage_collector.c \
    Garbage_Collector/garbage_collector1.c \
    Garbage_Collector/garbage_collector_parser.c \
    Garbage_Collector/garbage_collector_utils.c \
    Garbage_Collector/garbage_collector_utils1.c

# --- OBJECT FILES ---
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(RESET)"
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(READLINE_FLAGS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created successfully!$(RESET)"

$(LIBFT):
	@echo "$(YELLOW)Building libft...$(RESET)"
	$(MAKE) -C $(LIBFT_DIR)

# Rule to compile .c files into the obj/ directory
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(YELLOW)Full clean...$(RESET)"
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean
	$(MAKE) all

# Debug target
debug: CFLAGS += -g -DDEBUG
debug: re

sanitize: CFLAGS += -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -DDEBUG
sanitize: re

debug_san: CFLAGS += -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -DDEBUG
debug_san: re

.PHONY: all clean fclean re debug sanitize