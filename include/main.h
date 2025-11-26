/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 13:45:52 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
#define MAIN_H

#include "executor.h"
#include "garbage_collector.h"
#include "minishell.h"
#include "parser.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define PROMPT "minishell$ "

typedef struct s_redir_fds
{
	int saved_stdin;
	int saved_stdout;
	int in_fd;
	int out_fd;
} t_redir_fds;

typedef struct s_cmd_exec_data
{
	char **envp;
	char **merged_envp;
	t_redir_fds fds;
	int ret;
} t_cmd_exec_data;

/* main_loop.c */
int main_loop(t_env_list *env, t_shell_state *shell);

/* input_processing.c */
int process_command(t_cmd_list *cmdlst, t_env_list *envlst,
					t_shell_state *shell);
int process_input_line(char *line, t_env_list *env,
					   int last_status, t_shell_state *shell);
char *read_line_noninteractive(void);

/* env_utils.c */
char **env_list_to_array(t_env_list *env, t_shell_state *shell);
char ***cmdlist_to_array(t_cmd_list *cmdlst, t_shell_state *shell);

int is_empty_command(t_cmd_node *cmd);

/* env_merge.c */
char **merge_env_arrays(char **base_envp, t_env_list *cmd_env);

/* single_cmd_setup.c */
int setup_cmd_redirections(t_cmd_node *cmd, t_redir_fds *fds,
						   t_shell_state *shell);
void apply_cmd_redirections(t_redir_fds *fds, t_shell_state *shell);
void restore_cmd_fds(t_redir_fds *fds, t_shell_state *shell);
void cleanup_cmd_redir_failure(t_redir_fds *fds, char **envp,
							   char **merged, t_shell_state *shell);
int setup_and_check_command(t_cmd_node *cmd, t_env_list *env,
							t_shell_state *shell, t_cmd_exec_data *data);
int execute_command(t_cmd_node *cmd, t_shell_state *shell,
					t_cmd_exec_data *data);

/* single_cmd_env.c */
int handle_assignment_only(t_cmd_node *cmd, t_shell_state *shell);

/* single_cmd.c */
int handle_single_command(t_cmd_node *cmd, t_env_list *env,
						  t_shell_state *shell);

/* pipeline.c */
int handle_pipeline(t_cmd_list *cmdlst, t_env_list *env,
					t_shell_state *shell);

#endif