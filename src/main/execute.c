#include "minishell.h"
#include "executor.h"
#include <sys/wait.h>

static int is_builtin_cmd(char *s)
{
    if (!s)
        return (0);
    return (!ft_strcmp(s, "echo") || !ft_strcmp(s, "cd") || !ft_strcmp(s, "pwd") || !ft_strcmp(s, "export") || !ft_strcmp(s, "unset") || !ft_strcmp(s, "env"));
}

static int run_single_builtin(char **args, int in_fd, int out_fd)
{
    if (!args || !args[0] || !is_builtin_cmd(args[0]))
        return (-1);
    if (!ft_strcmp(args[0], "echo"))
        return (exec_builtin_with_redir(ft_echo, args, in_fd, out_fd));
    if (!ft_strcmp(args[0], "cd"))
        return (exec_builtin_with_redir(ft_cd, args, in_fd, out_fd));
    if (!ft_strcmp(args[0], "pwd"))
        return (exec_builtin_with_redir(ft_pwd, args, in_fd, out_fd));
    if (!ft_strcmp(args[0], "export"))
        return (exec_builtin_with_redir(ft_export, args, in_fd, out_fd));
    if (!ft_strcmp(args[0], "unset"))
        return (exec_builtin_with_redir(ft_unset, args, in_fd, out_fd));
    return (exec_builtin_with_redir(ft_env, args, in_fd, out_fd));
}

static int wait_child_status(pid_t pid)
{
    int status;

    status = 0;
    if (waitpid(pid, &status, 0) == -1)
        return (1);
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));
    return (1);
}

static void child_exec_external(char **args, int in_fd, int out_fd, char **envp)
{
    char *path;

    if (in_fd != -1)
        dup2(in_fd, STDIN_FILENO);
    if (out_fd != -1)
        dup2(out_fd, STDOUT_FILENO);
    if (in_fd != -1)
        close(in_fd);
    if (out_fd != -1)
        close(out_fd);
    path = find_in_path(args[0]);
    execve(path, args, envp);
    if (errno == ENOENT)
        _exit(127);
    if (errno == EACCES || errno == EISDIR)
        _exit(126);
    _exit(126);
}

static int run_single_external(char **args, int in_fd, int out_fd)
{
    pid_t pid;
    char **envp;

    envp = generate_env(g_shell.env);
    pid = fork();
    if (pid == 0)
        child_exec_external(args, in_fd, out_fd, envp);
    if (in_fd != -1)
        close(in_fd);
    if (out_fd != -1)
        close(out_fd);
    return (wait_child_status(pid));
}

static int count_cmds(t_cmd_list *lst)
{
    int n;
    t_cmd_node *cur;

    n = 0;
    cur = lst ? lst->head : NULL;
    while (cur)
    {
        n++;
        cur = cur->next;
    }
    return (n);
}

int execute_commands(t_cmd_list *commands)
{
    int ncmds;
    t_cmd_node *node;

    if (!commands)
        return (0);
    ncmds = count_cmds(commands);
    if (ncmds <= 0)
        return (0);
    if (ncmds > 1)
    {
        char ***cmdv;
        char **envp;
        int i;

        cmdv = gc_malloc(sizeof(char **) * ncmds);
        if (!cmdv)
            return (1);
        node = commands->head;
        i = 0;
        while (node)
        {
            cmdv[i++] = node->cmd;
            node = node->next;
        }
        envp = generate_env(g_shell.env);
        return (exec_pipeline(cmdv, ncmds, envp));
    }
    /* single command path */
    node = commands->head;
    if (!node || !node->cmd)
        return (0);
    {
        int in_fd;
        int out_fd;
        int red;

        in_fd = -1;
        out_fd = -1;
        red = setup_redirections(node->cmd, &in_fd, &out_fd);
        if (red == 2)
            return (2);
        if (red == 1)
            return (1);
        if (!node->cmd[0])
        {
            if (in_fd != -1)
                close(in_fd);
            if (out_fd != -1)
                close(out_fd);
            return (0);
        }
        {
            int bret;

            bret = run_single_builtin(node->cmd, in_fd, out_fd);
            if (bret >= 0)
                return (bret);
        }
        return (run_single_external(node->cmd, in_fd, out_fd));
    }
}
