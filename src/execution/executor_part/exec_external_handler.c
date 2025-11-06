/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:34:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/06 03:21:48 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Executes external command (execve in current process)
void exec_external(char **args, char **envp)
{
    char *exec_path;

    exec_path = find_in_path(args[0]);
    execve(exec_path, args, envp);
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
