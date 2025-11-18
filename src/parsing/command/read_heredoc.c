/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 13:46:13 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"
#include <readline/readline.h>
#include <unistd.h>

static char *append_line(char *content, char *line)
{
    char *new_content;
    char *with_newline;

    if (!line)
        return (content);
    with_newline = gc_strjoin(line, "\n");
    if (!with_newline)
        return (content);
    if (!content)
        return (with_newline);
    new_content = gc_strjoin(content, with_newline);
    if (!new_content)
        return (content);
    return (new_content);
}

static int is_delimiter(char *line, char *delimiter)
{
    size_t len;

    if (!line || !delimiter)
        return (0);
    len = ft_strlen(delimiter);
    if (ft_strncmp(line, delimiter, len) == 0 && line[len] == '\0')
        return (1);
    return (0);
}

char *read_heredoc_content(char *delimiter)
{
    char *content;
    char *line;

    if (!delimiter)
        return (NULL);
    if (!isatty(STDIN_FILENO))
        return (NULL);
    content = NULL;
    start_heredoc_signals();
    while (1)
    {
        line = readline("> ");
        if (!line || g_sigint_status == 130)
        {
            if (!line)
                write(2, "warning: here-document delimited by end-of-file\n", 49);
            start_signals();
            if (g_sigint_status == 130)
                return (NULL);
            return (content);
        }
        if (is_delimiter(line, delimiter))
            break;
        content = append_line(content, line);
    }
    start_signals();
    return (content);
}
