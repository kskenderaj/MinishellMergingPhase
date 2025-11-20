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
# include "garbage_collector.h"
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
// All parser types (env, cmd, file, token) are now defined in parser.h

// Structs -- BEGIN
typedef enum CMD_TYPE
{
	BUILTIN,
	CMD,
	PIPE
}								t_CMD_TYPE;

typedef enum REDIR_TYPE
{
	INFILE = 4,			// TK_INFILE
	OUTFILE = 5,		// TK_OUTFILE
	HEREDOC = 6,		// TK_HEREDOC
	OUTFILE_APPEND = 7,	// TK_APPEND
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
int	process_command(t_cmd_list *cmdlst, t_env_list *envlst);
char							*get_env_value(t_env_list *env,
									const char *key);
// main.c -- END

// signals.c -- BEGIN
void							handle_sig_int(int signal_nb);
void							remove_ctrlc_echo(void);
void							handle_ctrlc_heredoc(int signal_nb);
void							start_heredoc_signals(void);
void							start_signals(void);
void							reset_signals_for_child(void);
// signals.c -- END

// Functions -- END

#endif
