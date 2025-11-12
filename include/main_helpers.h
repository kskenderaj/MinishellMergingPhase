#ifndef MAIN_HELPERS_H
#define MAIN_HELPERS_H

#include "executor.h"
#include "parser.h"

char *get_input_line(void);
int is_comment_line(const char *s);
int is_exit_line(const char *s);
void collect_local_assignments(char **args, char **local_assigns);
void apply_assignments_when_no_command(char **local_assigns);
int detect_and_handle_pipeline(char **args, char **cmd_envp);
void run_command_or_builtin(char **args, char **cmd_envp, char **local_assigns, int in_fd, int out_fd);
char **apply_assignments_temp(char **local);
void restore_assignments(char **prev);
void expand_args_inplace(char **args);
int run_external(char **args, int in_fd, int out_fd, char **envp);
int build_and_run_pipeline(char **args, char **envp);
int execute_builtin_with_possible_redir(char **args, int in_fd, int out_fd);
int is_builtin(const char **args);

#endif
