/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 13:20:10 by kskender          #+#    #+#             */
/*   Updated: 2025/11/04 17:37:59 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "garbage_collector.h"
#include "libft.h"
#include "minishell.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Exported variables tracking*/
#ifndef MAX_EXPORTED
#define MAX_EXPORTED 128
#endif

typedef struct s_shell_state
{
	char *exported_vars[MAX_EXPORTED];
	int exported_count;
} t_shell_state;

/* global shell state is defined in globals.c */
extern t_shell_state g_shell;

typedef struct s_filelist
{
	char *filename;
	t_REDIR_TYPE type;
	struct s_filelist *next;
} t_filelist;

typedef struct s_commandlist
{
	t_filelist *files;
} t_commandlist;

typedef struct s_redir_data
{
	char **args;
	int *idx;
	int *in_fd;
	int *out_fd;
} t_redir_data;
/* Includes -- END */

/* Basics --Begin */
int outfile_redirector(t_file_node *file_node);
int infile_redirector(t_file_node *file_node);
void dup_and_or_close(int *prev_in_out, int *new_in_out);
void close_and_reset(int *prev_in_out, int *new_in_out,
					 int *reset);
int pipe_handler(t_cmd_node *command);
/* Basics --End */
/* Redirectories-- BEGIN */
int setup_output_file(t_commandlist *cmd);
int setup_output_file_from_cmd(t_cmd_node *cmd);
int count_input(t_commandlist *cmd);
int count_output(t_commandlist *cmd);
int setup_input_file(t_commandlist *cmd);
int setup_input_file_from_cmd(t_cmd_node *cmd);
int check_redirection(t_commandlist *cmd, int control,
					  int *reset);
/* Redirectories -- END */
/* Export utility functions -- BEGIN */
int is_valid_identifier(const char *name);
/* Builtins */
int ft_cd(char **args);
int ft_echo(char **args);
int ft_pwd(char **args);
int ft_env(char **args);
int ft_unset(char **args);
int ft_export(char **args);
void print_exported_env(void);
/* Builtins helpers */
char **generate_env(t_env_list *env);
int table_of_builtins(t_cmd_node *cmd, char **envp,
					  int flag);
/* External/parent */
void exec_external(char **args, char **envp);
int exec_pipeline(char ***cmds, int ncmds, char **envp);
int exec_builtin_with_redir(int (*builtin)(char **),
							char **args, int in_fd, int out_fd);
int exec_heredoc(const char *delimiter);
char **convert_env_to_array(t_env_list *env_list);
char *find_in_path(char *cmd);
void shift_left_by(char **args, int start, int by);
int handle_attached_operators(t_redir_data *data,
							  char *tok);
int handle_separated_operators(char **args, int *i,
							   int *in_fd, int *out_fd);
pid_t exec_parent_runner(t_cmd_node *cmd, int *io_data);
/* Main utilities */
void split_args(char *input, char **args, int max_args);
int setup_redirections(char **args, int *in_fd,
					   int *out_fd);
/* Initialization */
void init_shell(void);
/* Utility to run */
int not_error_file(t_filelist *current, int update,
				   t_commandlist *cmd);
int standard_error(int update, t_commandlist *cmd);
int handling_the_infile(t_commandlist *cmd,
						t_filelist *current, int update);
int handling_the_heredoc(t_commandlist *cmd,
						 t_filelist *current, int update);
int setup_input(t_commandlist *cmd, int *redirect,
				int update);
/* Redir infile/outfile/append/heredoc helpers */
int count_infile(t_commandlist *cmd);
int setup_infile_fd(t_commandlist *cmd);
int count_outfile(t_commandlist *cmd);
int setup_outfile_fd(t_commandlist *cmd);
int count_heredoc(t_commandlist *cmd);
int setup_heredoc_fd(t_commandlist *cmd);
/* Export utility functions -- END */
#endif
