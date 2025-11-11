/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:22:44 by kskender          #+#    #+#             */
/*   Updated: 2025/11/11 01:57:48 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

/*
** =============================================================================
**   SYSTEM LIBRARIES
** =============================================================================
*/

#include "libft/libft.h"
#include <dirent.h>
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
** =============================================================================
**   MACROS
** =============================================================================
*/

#define PROMPT "minishell$ "

/*
** =============================================================================
**   CORE DATA STRUCTURES & ENUMS (DEFINE THESE FIRST!)
** =============================================================================
*/

// Forward-declare structs to prevent dependency issues
struct s_env_list;

// Enum for command types
typedef enum e_CMD_TYPE
{
	BUILTIN,
	CMD,
	PIPE
} t_CMD_TYPE;

// Enum for redirection types
typedef enum e_REDIR_TYPE
{
	INFILE,
	OUTFILE,
	HEREDOC,
	OUTFILE_APPEND,
	NO_REDIRECTION = -1
} t_REDIR_TYPE;

// Node for a file redirection
typedef struct s_file_node
{
	char *filename;
	int redir_type; // Should be t_REDIR_TYPE
	struct s_file_node *next;
} t_file_node;

// List of file redirections
typedef struct s_file_list
{
	t_file_node *head;
	t_file_node *tail;
	ssize_t size;
} t_file_list;

// Node for a single command
typedef struct s_cmd_node
{
	struct s_cmd_node *next;
	int cmd_type; // Should be t_CMD_TYPE
	char **cmd;
	t_file_list *files;
	struct s_env_list *env;
} t_cmd_node;

// List of commands
typedef struct s_cmd_list
{
	int syntax_error;
	t_cmd_node *head;
	t_cmd_node *tail;
	ssize_t size;
} t_cmd_list;

// Node for an environment variable
typedef struct s_env_node
{
	char *key;
	char *value;
	struct s_env_node *next;
} t_env_node;

// List of environment variables
typedef struct s_env_list
{
	t_env_node *head;
	t_env_node *tail;
	ssize_t size;
	pid_t *pid;
} t_env_list;

/*
** =============================================================================
**   GLOBAL VARIABLE & PROJECT HEADERS (INCLUDE AFTER STRUCTS)
** =============================================================================
*/

extern volatile sig_atomic_t g_sigint_status;

#include "parser.h"
#include "executor.h"
// # include "garbage_collector.h" // Uncomment if you have this header

/*
** =============================================================================
**   FUNCTION PROTOTYPES
** =============================================================================
*/

// exit_code.c
int *exit_code(void);

// main.c
t_env_list *setup_env_list(char **envp);
t_env_list *initialize_shell(char **env);
char *get_prompt(void);
int process_command(char *prompt, t_env_list *env_list);
char *get_env_value(t_env_list *env, const char *key);

// signals.c
void handle_sig_int(int signal_nb);
void remove_ctrlc_echo(void);
void handle_ctrlc_heredoc(int signal_nb);
void start_heredoc_signals(void);
void start_signals(void);

#endif