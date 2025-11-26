/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:22:44 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 13:47:06 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

// Includes -- BEGIN
#include "garbage_collector.h"
#include "libft/libft.h"
#include "parser.h"
#include <dirent.h>
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
// Includes -- END

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// All parser types (env, cmd, file, token) are now defined in parser.h

// Structs -- BEGIN
typedef enum CMD_TYPE
{
	BUILTIN,
	CMD,
	PIPE
} t_CMD_TYPE;

typedef enum REDIR_TYPE
{
	INFILE = 4,
	OUTFILE = 5,
	HEREDOC = 6,
	OUTFILE_APPEND = 7,
	NO_REDIRECTION = -42
} t_redir_type;
// Structs -- END

// Functions -- BEGIN

// exit_code.c -- BEGIN
int *exit_code(void);
// exit_code.c -- END

// main.c -- BEGIN
t_env_list *setup_env_list(void);
t_env_list *initialize_shell(char **env);
char *get_prompt(void);
char *get_env_value(t_env_list *env,
					const char *key);
// main.c -- END

// signals.c -- BEGIN
extern volatile sig_atomic_t g_signal_status;
void sigint_exit(int sig);
void within_command(void);
void out_command(void);
void parent_sigint_handler(int signo);
void heredoc_sigint_handler(int sig);
void set_signals_parent(void);
void set_signals_child(void);
void set_signals_heredoc(void);
void reset_signals_default(void);
// signals.c -- END

// Functions -- END

#endif