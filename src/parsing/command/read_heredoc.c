/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:35:37 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "executor.h"
#include <readline/readline.h>
#include <unistd.h>

char	*append_line(char *content, char *line, t_shell_state *shell)
{
	char	*new_content;
	char	*with_newline;

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

int	is_delimiter(char *line, char *delimiter)
{
	size_t	len;

	if (!line || !delimiter)
		return (0);
	len = ft_strlen(delimiter);
	if (len == 0)
		return (0);
	if (ft_strncmp(line, delimiter, len) == 0 && line[len] == '\0')
		return (1);
	return (0);
}

int	handle_heredoc_line(char **content, char *line, char *delimiter,
		t_shell_state *shell)
{
	if (g_sigint_status == 130)
	{
		if (line)
			free(line);
		return (-1);
	}
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

char	*read_heredoc_content(char *delimiter, t_shell_state *shell)
{
	char	*content;
	char	*line;
	int		ret;

	if (!delimiter || !isatty(STDIN_FILENO))
		return (NULL);
	content = NULL;
	while (1)
	{
		line = readline("> ");
		ret = handle_heredoc_line(&content, line, delimiter, shell);
		if (ret == -1)
		{
			return (NULL);
		}
		if (ret == 1 || ret == 2)
			break ;
	}
	return (content);
}
