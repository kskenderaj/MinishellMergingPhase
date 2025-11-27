/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/27 18:19:43 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"
#include <readline/readline.h>
#include <unistd.h>

static int check_heredoc_interrupt(void)
{
	if (g_signal_status == 130)
	{
		rl_done = 1;
		return (0);
	}
	return (0);
}

char *append_line(char *content, char *line, t_shell_state *shell)
{
	char *new_content;
	char *with_newline;

	if (!line)
		return (content);
	with_newline = gc_strjoin(shell->gc, line, "\n");
	if (!with_newline)
		return (content);
	if (!content)
		return (with_newline);
	new_content = gc_strjoin(shell->gc, content, with_newline);
	if (!new_content)
		return (content);
	return (new_content);
}

int handle_heredoc_line(char **content, char *line, char *delimiter,
						t_shell_state *shell)
{
	if (!line)
	{
		write(2, "warning: here-document delimited by end-of-file\n", 49);
		return (1);
	}
	if (is_delimiter(line, delimiter))
	{
		free(line);
		return (2);
	}
	*content = append_line(*content, line, shell);
	free(line);
	return (0);
}

static int check_signal_and_read(char **line)
{
	if (g_signal_status == 130)
	{
		rl_event_hook = NULL;
		return (1);
	}
	*line = readline("> ");
	if (g_signal_status == 130)
	{
		if (*line)
			free(*line);
		rl_event_hook = NULL;
		return (1);
	}
	return (0);
}

char *read_heredoc_content(char *delimiter, t_shell_state *shell)
{
	char *content;
	char *line;
	int ret;

	set_signals_heredoc();
	if (!delimiter || !isatty(STDIN_FILENO))
		return (NULL);
	rl_event_hook = check_heredoc_interrupt;
	content = NULL;
	while (1)
	{
		if (check_signal_and_read(&line))
			return (set_signals_parent(), NULL);
		ret = handle_heredoc_line(&content, line, delimiter, shell);
		if (ret == 1 || ret == 2)
			break;
		if (!line)
		{
			rl_event_hook = NULL;
			set_signals_parent();
			return (NULL);
		}
	}
	rl_event_hook = NULL;
	return (set_signals_parent(), content);
}
