/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler3.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:13:54 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:14:49 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

char	*process_line(char *content, int start, int i, t_shell_state *shell)
{
	char	*line;
	char	*expanded;
	char	*result;

	result = gc_strdup(shell->gc, "");
	line = gc_substr(shell->gc, content, start, i - start);
	expanded = expand_heredoc_line(line, 1, shell);
	result = gc_strjoin(shell->gc, result, expanded);
	return (result);
}

void	heredoc_read_loop(const char *delimiter, int quoted,
		int pipefd[2], int is_tty, t_shell_state *shell)
{
	char	*buffer;
	size_t	delim_len;

	delim_len = ft_strlen(delimiter);
	if (delim_len == 0)
		delim_len = (size_t) - 1;
	while (1)
	{
		buffer = read_heredoc_buffer(is_tty, shell);
		if (!buffer)
		{
			handle_heredoc_eof(is_tty, pipefd, shell);
			return ;
		}
		if (check_delimiter(buffer, delimiter, delim_len, is_tty))
			break ;
		process_heredoc_line(buffer, quoted, pipefd, is_tty, shell);
	}
}
