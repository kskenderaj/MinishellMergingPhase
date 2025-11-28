/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 13:20:10 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 13:44:38 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "garbage_collector.h"
# include "libft.h"
# include "minishell.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>

/* Exported variables tracking*/
# ifndef MAX_EXPORTED
#  define MAX_EXPORTED 128
# endif

typedef struct s_file_node	t_file_node;
typedef struct s_cmd_node	t_cmd_node;
typedef struct s_env_list	t_env_list;
typedef enum REDIR_TYPE		t_redir_type;

typedef struct s_shell_state
{
	char					*exported_vars[MAX_EXPORTED];
	int						exported_count;
	int						last_status;
	struct s_env_list		*env;
	int						is_interactive;
	char					**current_envp;
	char					*current_line;
	struct s_gc				*gc;
}							t_shell_state;

typedef struct s_filelist
{
	char					*filename;
	t_redir_type			type;
	struct s_filelist		*next;
}							t_filelist;

typedef struct s_commandlist
{
	t_filelist				*files;
}							t_commandlist;

typedef struct s_heredoc_ctx
{
	char					**result;
	char					*content;
	int						*start;
	int						i;
	t_shell_state			*shell;
	const char				*delimiter;
	int						quoted;
	int						*pipefd;
	int						is_tty;
}							t_heredoc_ctx;

typedef struct s_parse_state
{
	char					*scan;
	char					*p;
	int						total;
	char					first_quote;
	int						all_quoted;
}							t_parse_state;

typedef struct s_redir_data
{
	char					**args;
	int						*idx;
	int						*in_fd;
	int						*out_fd;
}							t_redir_data;

typedef struct s_pipeline_ctx
{
	t_cmd_list				*cmdlst;
	int						ncmds;
	char					**envp;
	int						**pipes;
	pid_t					*pids;
	char					***per_cmd_envs;
}							t_pipeline_ctx;

typedef struct s_child_ctx
{
	int						idx;
	int						ncmds;
	int						**pipes;
	int						in_fd;
	int						out_fd;
	t_cmd_node				*cmd_node;
	char					**cmd;
	char					**envp;
}							t_child_ctx;

typedef struct s_child_init
{
	t_child_ctx				*child;
	t_pipeline_ctx			*ctx;
	int						i;
	int						in_fd;
	int						out_fd;
	t_cmd_node				*node;
	t_shell_state			*shell;
}							t_child_init;
/* Includes -- END */

/* Redirectories-- BEGIN */
int							setup_output_file(t_commandlist *cmd,
								t_shell_state *shell);
int							setup_output_file_from_cmd(t_cmd_node *cmd,
								t_shell_state *shell);
int							count_input(t_commandlist *cmd);
int							count_output(t_commandlist *cmd);
int							setup_input_file(t_commandlist *cmd,
								t_shell_state *shell);
int							setup_input_file_from_cmd(t_cmd_node *cmd,
								t_shell_state *shell);
int							check_redirection(t_commandlist *cmd, int control,
								int *reset, t_shell_state *shell);
int							open_all_output_files(t_commandlist *cmd,
								t_shell_state *shell);
/* Redirectories -- END */
/* Export utility functions -- BEGIN */
int							is_valid_identifier(const char *name);
/* Builtins */
int							ft_cd(char **args, t_shell_state *shell);
int							ft_echo(char **args, t_shell_state *shell);
int							ft_pwd(char **args, t_shell_state *shell);
int							ft_env(char **args, t_shell_state *shell);
int							ft_unset(char **args, t_shell_state *shell);
int							ft_export(char **args, t_shell_state *shell);
int							ft_exit(char **args, t_shell_state *shell);
void						print_exported_env(t_shell_state *shell);
/* Builtins helpers */
char						**generate_env(t_env_list *env,
								t_shell_state *shell);
int							table_of_builtins(t_cmd_node *cmd, char **envp,
								int flag, t_shell_state *shell);
void						update_shell_env(const char *name,
								const char *value, t_shell_state *shell);
char						*strip_quotes(const char *value,
								t_shell_state *shell);
int							is_in_exported(const char *name,
								t_shell_state *shell);
int							export_invalid_error(char *name,
								t_shell_state *shell);
void						set_export_value(char *name, char *value,
								t_shell_state *shell);
/* External/parent */
void						exec_external(char **args, char **envp,
								t_shell_state *shell);
int							exec_pipeline(t_cmd_list *cmdlst, char **envp,
								t_shell_state *shell);
int							exec_builtin_with_redir(int (*builtin)(char **,
									t_shell_state *), char **args, int in_fd,
								int out_fd, t_shell_state *shell);
int							exec_heredoc(const char *delimiter, int quoted,
								t_shell_state *shell);
int							exec_heredoc_from_content(char *content, int quoted,
								t_shell_state *shell);
char						*expand_heredoc_line(char *line, int should_expand,
								t_shell_state *shell);
char						*read_line_from_stdin(t_shell_state *shell);
char						*process_line(char *content, int start, int i,
								t_shell_state *shell);
char						*expand_heredoc_content(char *content, int quoted,
								t_shell_state *shell);
char						*read_heredoc_buffer(int is_tty,
								t_shell_state *shell);
int							check_delimiter(char *buffer, const char *delimiter,
								size_t delim_len, int is_tty);
int							handle_heredoc_interrupt(char *buffer,
								int pipefd[2], t_shell_state *shell);
int							handle_heredoc_eof(int is_tty, int pipefd[2],
								t_shell_state *shell);
void						process_heredoc_line(t_heredoc_ctx *ctx);
void						heredoc_read_loop(t_heredoc_ctx *ctx);
int							create_pipes(int **pipes, int ncmds,
								t_shell_state *shell);
void						close_all_pipes(int **pipes, int ncmds,
								t_shell_state *shell);
int							wait_children(pid_t *pids, int ncmds,
								t_shell_state *shell);
int							spawn_pipeline_children(t_pipeline_ctx *ctx,
								t_shell_state *shell);
void						setup_child_io_and_exec(t_child_ctx *ctx,
								t_shell_state *shell);
char						**get_cmd_env(t_pipeline_ctx *ctx, int i);
void						init_child_ctx(t_child_init *init);
void						close_fds(int in_fd, int out_fd,
								t_shell_state *shell);
void						apply_env_vars(char **envp, t_shell_state *shell);
void						execute_builtin(char **cmd, t_shell_state *shell);
char						**convert_env_to_array(t_env_list *env_list,
								t_shell_state *shell);
char						*find_in_path(char *cmd, t_shell_state *shell);
void						shift_left_by(char **args, int start, int by);
int							handle_attached_operators(t_redir_data *data,
								char *tok, t_shell_state *shell);
int							handle_separated_operators(t_redir_data *data,
								t_shell_state *shell);
pid_t						exec_parent_runner(t_cmd_node *cmd, int *io_data,
								t_shell_state *shell);
/* Main utilities */
void						split_args(char *input, char **args, int max_args,
								t_shell_state *shell);
void						init_parse_state(t_parse_state *state, char **str,
								char delim);
void						handle_escape_seq(t_parse_state *state);
void						handle_quoted_segment(t_parse_state *state);
int							calculate_word_length(t_parse_state *state,
								char delim);
char						*allocate_result_buffer(t_parse_state *state,
								t_shell_state *shell);
void						copy_escape_char(char **p, char *out, int *idx,
								int marker);
void						copy_quoted_content(char **p, char *out, int *idx,
								int marker);
void						fill_result_buffer(t_parse_state *state,
								char *result, char delim);
int							setup_redirections(char **args, int *in_fd,
								int *out_fd, t_shell_state *shell);
/* Initialization */
void						init_shell(t_shell_state *shell);
void						cleanup_shell(t_shell_state *shell);
/* Utility to run */
int							not_error_file(t_filelist *current, int update,
								t_commandlist *cmd);
int							standard_error(int update, t_commandlist *cmd);
int							handling_the_infile(t_commandlist *cmd,
								t_filelist *current, int update,
								t_shell_state *shell);
int							handling_the_heredoc(t_commandlist *cmd,
								t_filelist *current, int update,
								t_shell_state *shell);
int							setup_input(t_commandlist *cmd, int *redirect,
								int update, t_shell_state *shell);
/* Redir infile/outfile/append/heredoc helpers */
int							count_infile(t_commandlist *cmd);
int							setup_infile_fd(t_commandlist *cmd,
								t_shell_state *shell);
int							count_outfile(t_commandlist *cmd);
int							setup_outfile_fd(t_commandlist *cmd,
								t_shell_state *shell);
int							count_heredoc(t_commandlist *cmd);
int							setup_heredoc_fd(t_commandlist *cmd,
								t_shell_state *shell);
int							open_all_output_files(t_commandlist *cmd,
								t_shell_state *shell);
/* Export utility functions -- END */
#endif
