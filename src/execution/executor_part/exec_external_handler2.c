/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:13:51 by kskender          #+#    #+#             */
/*   Updated: 2025/11/27 17:27:17 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

char *read_heredoc_buffer(int is_tty, t_shell_state *shell)
{
	char *buffer;

	if (is_tty)
	{
		buffer = readline("> ");
		if (g_signal_status == 130)
			return (NULL);
		return (buffer);
	}
	else
		return (read_line_from_stdin(shell));
}

int check_delimiter(char *buffer, const char *delimiter, size_t delim_len,
					int is_tty)
{
	if (ft_strncmp(buffer, delimiter, delim_len) == 0 && buffer[delim_len] == '\0')
	{
		if (is_tty)
			free(buffer);
		return (1);
	}
	return (0);
}

int handle_heredoc_interrupt(char *buffer, int pipefd[2], t_shell_state *shell)
{
	(void)shell;
	if (buffer)
		free(buffer);
	close(pipefd[1]);
	close(pipefd[0]);
	return (-2);
}

int handle_heredoc_eof(int is_tty, int pipefd[2], t_shell_state *shell)
{
	(void)shell;
	if (is_tty)
		write(STDERR_FILENO,
			  "warning: here-document delimited by end-of-file\n", 49);
	close(pipefd[1]);
	return (pipefd[0]);
}

void process_heredoc_line(t_heredoc_ctx *ctx)
{
	char *expanded;

	expanded = expand_heredoc_line(*ctx->result, !ctx->quoted, ctx->shell);
	write(ctx->pipefd[1], expanded, ft_strlen(expanded));
	write(ctx->pipefd[1], "\n", 1);
	if (ctx->is_tty)
		free(*ctx->result);
}
