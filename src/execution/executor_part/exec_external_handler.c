/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:34:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 20:13:52 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <errno.h>
#include <string.h>

static int has_slash(const char *s)
{
    int i;

    if (!s)
        return (0);
    i = 0;
    while (s[i])
    {
        if (s[i] == '/')
            return (1);
        i++;
    }
    return (0);
}

static void print_exec_error_and_exit(char **args)
{
    int code;

    code = 126;
    if (errno == ENOENT)
    {
        if (has_slash(args[0]))
        {
            ft_putstr_fd("minishell: ", STDERR_FILENO);
            ft_putstr_fd(args[0], STDERR_FILENO);
            ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
        }
        else
        {
            ft_putstr_fd("minishell: ", STDERR_FILENO);
            ft_putstr_fd(args[0], STDERR_FILENO);
            ft_putstr_fd(": command not found\n", STDERR_FILENO);
        }
        code = 127;
    }
    else if (errno == EACCES)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(args[0], STDERR_FILENO);
        ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
        code = 126;
    }
    else if (errno == EISDIR)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(args[0], STDERR_FILENO);
        ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
        code = 126;
    }
    else
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(args[0], STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putstr_fd((char *)strerror(errno), STDERR_FILENO);
        ft_putstr_fd("\n", STDERR_FILENO);
        code = 126;
    }
    _exit(code);
}

// Executes external command (execve in current process)
void exec_external(char **args, char **envp)
{
    char *exec_path;
    if (!args || !args[0] || args[0][0] == '\0')
    {
        errno = ENOENT;
        /* fabricate a placeholder name so error line matches tester expectations */
        static char *placeholder[2] = {"", NULL};
        print_exec_error_and_exit(placeholder);
    }
    exec_path = find_in_path(args[0]);
    execve(exec_path, args, envp);
    print_exec_error_and_exit(args);
}

// Sets up redirections for builtins and runs them
int exec_builtin_with_redir(int (*builtin)(char **), char **args, int in_fd, int out_fd)
{
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    int ret;
    if (in_fd != -1)
    {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }
    if (out_fd != -1)
    {
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
    ret = builtin(args);
    g_shell.last_status = ret;
    dup2(saved_in, STDIN_FILENO);
    dup2(saved_out, STDOUT_FILENO);
    close(saved_in);
    close(saved_out);
    return ret;
}

// Handles heredoc input (stub, expand as needed)
int exec_heredoc(const char *delimiter)
{
    char buffer[1024];
    int pipefd[2];

    if (pipe(pipefd) == -1)
        return (-1);
    while (1)
    {
        if (!fgets(buffer, sizeof(buffer), stdin))
            break;
        /* line ends with newline from fgets */
        if (strncmp(buffer, delimiter, strlen(delimiter)) == 0 && buffer[strlen(delimiter)] == '\n')
            break;
        write(pipefd[1], buffer, strlen(buffer));
    }
    close(pipefd[1]);
    return (pipefd[0]); /* read end */
}
