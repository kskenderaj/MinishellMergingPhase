/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 20:12:07 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 02:41:23 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "builtins.h"
#include "main_helpers.h" /* for expand_args_inplace prototype */
#include <sys/wait.h>

int is_builtin_cmd(char *s)
{
    if (!s)
        return (0);
    return (!ft_strcmp(s, "echo") || !ft_strcmp(s, "cd") || !ft_strcmp(s, "pwd") || !ft_strcmp(s, "export") || !ft_strcmp(s, "unset") || !ft_strcmp(s, "env") || !ft_strcmp(s, "exit"));
}

static int run_single_builtin(char **args, int in_fd, int out_fd)
{
    if (!args || !args[0] || !is_builtin_cmd(args[0]))
        return (-1);
    expand_args_inplace(args);
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
    if (!ft_strcmp(args[0], "exit"))
        return (exec_builtin_with_redir(ft_exit, args, in_fd, out_fd));
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
    start_signals(); /* ensure parent prompt signals aren't inherited incorrectly */
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    if (!args || !args[0] || args[0][0] == '\0')
    {
        extern void exec_external(char **args, char **envp);
        exec_external(args, envp);
    }
    path = find_in_path(args[0]);
    execve(path, args, envp);
    /* Reuse external handler's error printing for consistency */
    {
        extern void exec_external(char **args, char **envp);
        (void)path;
        /* exec_external also prints and exits on failure */
        exec_external(args, envp);
    }
}

static int run_single_external(char **args, int in_fd, int out_fd)
{
    pid_t pid;
    char **envp;

    expand_args_inplace(args);
    envp = generate_env(g_shell.env);
    pid = fork();
    if (pid == 0)
        child_exec_external(args, in_fd, out_fd, envp);
    if (in_fd != -1)
        close(in_fd);
    if (out_fd != -1)
        close(out_fd);
    {
        int st = wait_child_status(pid);
        g_shell.last_status = st;
        return (st);
    }
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
            /* expand args for each command in pipeline */
            if (node->cmd)
                expand_args_inplace(node->cmd);
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
        /* expand before handling redirections so files can use $VAR */
        if (node->cmd)
            expand_args_inplace(node->cmd);
        red = setup_redirections(node->cmd, &in_fd, &out_fd);
        if (red == 2)
        {
            g_shell.last_status = 2;
            return (2);
        }
        if (red == 1)
        {
            g_shell.last_status = 1;
            return (1);
        }
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
            {
                g_shell.last_status = bret;
                return (bret);
            }
        }
        return (run_single_external(node->cmd, in_fd, out_fd));
    }
}
