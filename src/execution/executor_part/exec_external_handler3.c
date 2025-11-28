/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler3.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:13:54 by kskender          #+#    #+#             */
/*   Updated: 2025/11/28 13:29:27 by jtoumani         ###   ########.fr       */
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

void	heredoc_read_loop(t_heredoc_ctx *ctx)
{
	char	*buffer;
	size_t	delim_len;

	delim_len = ft_strlen(ctx->delimiter);
	if (delim_len == 0)
		delim_len -= 1;
	while (1)
	{
		if (g_signal_status == 130)
		{
			g_signal_status = 0;
			close(ctx->pipefd[1]);
			return ;
		}
		buffer = read_heredoc_buffer(ctx->is_tty, ctx->shell);
		if (!buffer)
		{
			handle_heredoc_eof(ctx->is_tty, ctx->pipefd, ctx->shell);
			return ;
		}
		if (check_delimiter(buffer, ctx->delimiter, delim_len, ctx->is_tty))
			break ;
		ctx->result = &buffer;
		process_heredoc_line(ctx);
	}
}
