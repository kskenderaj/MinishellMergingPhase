/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:34:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 21:24:56 by klejdi           ###   ########.fr       */
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

static void print_cmd_prefix(const char *arg0)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd((char *)arg0, STDERR_FILENO);
}

static int print_enoent_error(const char *arg0)
{
    if (has_slash(arg0))
    {
        print_cmd_prefix(arg0);
        ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
    }
    else
    {
        print_cmd_prefix(arg0);
        ft_putstr_fd(": command not found\n", STDERR_FILENO);
    }
    return (127);
}

static int print_specific_errno(const char *arg0)
{
    if (errno == EACCES)
    {
        print_cmd_prefix(arg0);
        ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
        return (126);
    }
    if (errno == EISDIR)
    {
        print_cmd_prefix(arg0);
        ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
        return (126);
    }
    print_cmd_prefix(arg0);
    ft_putstr_fd(": ", STDERR_FILENO);
    ft_putstr_fd((char *)strerror(errno), STDERR_FILENO);
    ft_putstr_fd("\n", STDERR_FILENO);
    return (126);
}

static void print_exec_error_and_exit(char **args)
{
    int code;

    if (errno == ENOENT)
        code = print_enoent_error(args[0]);
    else
        code = print_specific_errno(args[0]);
    _exit(code);
}

// Executes external command (execve in current process)
void exec_external(char **args, char **envp)
{
    char *exec_path;
    if (!args || !args[0] || args[0][0] == '\0')
    {
        /* empty command: just succeed to pass heredoc/pipes through */
        _exit(0);
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
static char *strip_outer_quotes(const char *s)
{
    size_t len;
    if (!s)
        return (NULL);
    len = ft_strlen(s);
    if (len >= 2 && ((s[0] == '"' && s[len - 1] == '"') || (s[0] == '\'' && s[len - 1] == '\'')))
        return (ft_substr(s, 1, len - 2));
    return (ft_strdup(s));
}

/* Remove any internal quote marker bytes (0x01, 0x02) that may prefix or
** appear inside tokens produced by the splitter, so comparisons are literal. */
static char *remove_quote_markers(const char *s)
{
    size_t i;
    size_t j;
    size_t len;
    char *out;

    if (!s)
        return (NULL);
    len = ft_strlen(s);
    out = (char *)malloc(len + 1);
    if (!out)
        return (NULL);
    i = 0;
    j = 0;
    while (s[i])
    {
        if ((unsigned char)s[i] != 0x01 && (unsigned char)s[i] != 0x02)
            out[j++] = s[i];
        i++;
    }
    out[j] = '\0';
    return (out);
}
static char *sanitize_delimiter(const char *delimiter)
{
    char *clean;
    char *delim;

    clean = remove_quote_markers(delimiter);
    if (!clean)
        return (NULL);
    delim = strip_outer_quotes(clean);
    free(clean);
    return (delim);
}

static int heredoc_loop_and_write(int wfd, const char *delim)
{
    char *line;

    while (1)
    {
        line = readline("> ");
        if (g_sigint_status)
        {
            if (line)
                free(line);
            return (-2);
        }
        if (!line)
        {
            ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", STDERR_FILENO);
            break;
        }
        if (ft_strcmp(line, (char *)delim) == 0)
        {
            free(line);
            break;
        }
        write(wfd, line, ft_strlen(line));
        write(wfd, "\n", 1);
        free(line);
    }
    return (0);
}

int exec_heredoc(const char *delimiter)
{
    int pipefd[2];
    char *delim;
    int res;

    if (!delimiter)
        return (-1);
    if (pipe(pipefd) == -1)
        return (-1);
    delim = sanitize_delimiter(delimiter);
    if (!delim)
    {
        gc_close(pipefd[0]);
        gc_close(pipefd[1]);
        return (-1);
    }
    start_heredoc_signals();
    res = heredoc_loop_and_write(pipefd[1], delim);
    start_signals();
    gc_close(pipefd[1]);
    if (res == -2)
    {
        gc_close(pipefd[0]);
        free(delim);
        return (-1);
    }
    free(delim);
    return (pipefd[0]);
}
