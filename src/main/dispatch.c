#include "main_helpers.h"
#include "executor.h"

int detect_and_handle_pipeline(char **args, char **cmd_envp)
{
    int i = 0;
    while (args[i])
    {
        if (!strcmp(args[i], "|"))
        {
            build_and_run_pipeline(args, cmd_envp);
            return (1);
        }
        i++;
    }
    return (0);
}

void run_command_or_builtin(char **args, char **cmd_envp, char **local_assigns, int in_fd, int out_fd)
{
    char **prev = NULL;

    if (args[0] && args[0][0] == '(')
    {
        int argc = 0;
        while (args[argc])
            argc++;
        if (argc > 0 && args[argc - 1] && args[argc - 1][ft_strlen(args[argc - 1]) - 1] == ')')
        {
            expand_args_inplace(args);
            args[0] = args[0] + 1;
            args[argc - 1][ft_strlen(args[argc - 1]) - 1] = '\0';
            pid_t pid = fork();
            if (pid == 0)
            {
                if (is_builtin((const char **)args))
                {
                    int ret = execute_builtin_with_possible_redir(args, in_fd, out_fd);
                    exit(ret);
                }
                else
                {
                    run_external(args, in_fd, out_fd, cmd_envp);
                    exit(g_shell.last_status);
                }
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                    g_shell.last_status = WEXITSTATUS(status);
                else if (WIFSIGNALED(status))
                    g_shell.last_status = 128 + WTERMSIG(status);
                else
                    g_shell.last_status = 128;
            }
            return;
        }
    }

    if (is_builtin((const char **)args))
    {
        if (local_assigns[0])
            prev = apply_assignments_temp(local_assigns);
        expand_args_inplace(args);
        execute_builtin_with_possible_redir(args, in_fd, out_fd);
        if (prev)
            restore_assignments(prev);
        return;
    }
    expand_args_inplace(args);
    run_external(args, in_fd, out_fd, cmd_envp);
}
