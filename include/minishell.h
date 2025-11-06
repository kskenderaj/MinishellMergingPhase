/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:22:44 by kskender          #+#    #+#             */
/*   Updated: 2025/11/03 15:41:13 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// Includes -- BEGIN
// # include "executor.h"
// # include "garbage_collector.h"
# include "libft/libft.h"
# include "parser.h"
# include <dirent.h>
# include <errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <sys/ioctl.h>
# include <termios.h>
// Includes -- END
typedef struct s_file_node
{
	char						*filename;
	int							redir_type;
	struct s_file_node			*next;
}								t_file_node;

typedef struct s_file_list
{
	t_file_node					*head;
	t_file_node					*tail;
	ssize_t						size;
}								t_file_list;

typedef struct s_cmd_node
{
	struct s_cmd_node			*next;
	int							cmd_type;
	char						**cmd;
	t_file_list					*files;
	struct s_env_list			*env;
}								t_cmd_node;

typedef struct s_cmd_list
{
	int							syntax_error;
	t_cmd_node					*head;
	t_cmd_node					*tail;
	ssize_t						size;
}								t_cmd_list;

typedef struct s_env_node
{
	char						*key;
	char						*value;
	struct s_env_node			*next;
}								t_env_node;

typedef struct s_env_list
{
	t_env_node					*head;
	t_env_node					*tail;
	ssize_t						size;
	pid_t						*pid;
}								t_env_list;

// Structs -- BEGIN
typedef enum CMD_TYPE
{
	BUILTIN,
	CMD,
	PIPE
}								t_CMD_TYPE;

typedef enum REDIR_TYPE
{
	INFILE,
	OUTFILE,
	HEREDOC,
	OUTFILE_APPEND,
	NO_REDIRECTION = -1
}								t_REDIR_TYPE;
// Structs -- END

extern volatile sig_atomic_t	g_sigint_status;

// Functions -- BEGIN

// exit_code.c -- BEGIN
int								*exit_code(void);
// exit_code.c -- END

// main.c -- BEGIN
t_env_list						*setup_env_list(void);
t_env_list						*initialize_shell(char **env);
char							*get_prompt(void);
int								process_command(char *prompt,
									t_env_list *env_list);
char							*get_env_value(t_env_list *env,
									const char *key);
// main.c -- END

// signals.c -- BEGIN
void							handle_sig_int(int signal_nb);
void							remove_ctrlc_echo(void);
void							handle_ctrlc_heredoc(int signal_nb);
void							start_heredoc_signals(void);
void							start_signals(void);
// signals.c -- END

// Functions -- END

#endif
