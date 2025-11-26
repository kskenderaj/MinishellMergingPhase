/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler1.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:13:19 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 14:51:36 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

char *expand_heredoc_line(char *line, int should_expand,
						  t_shell_state *shell)
{
	t_segment_list *seglst;
	char *expanded;

	if (!should_expand || !line)
		return (line);
	seglst = gc_malloc(shell->gc, sizeof(*seglst));
	if (!seglst)
		return (line);
	init_segment_lst(seglst);
	if (!find_segment(seglst, line, shell))
		return (line);
	expanded = segments_expand(seglst, shell->env, shell->last_status, shell);
	if (!expanded)
		return (line);
	return (expanded);
}

static void process_newline(t_heredoc_ctx *ctx)
{
	char *line;
	char *expanded;

	line = gc_substr(ctx->shell->gc, ctx->content, *ctx->start,
					 ctx->i - *ctx->start);
	expanded = expand_heredoc_line(line, 1, ctx->shell);
	*ctx->result = gc_strjoin(ctx->shell->gc, *ctx->result, expanded);
	*ctx->result = gc_strjoin(ctx->shell->gc, *ctx->result, "\n");
	*ctx->start = ctx->i + 1;
}

char *expand_heredoc_content(char *content, int quoted, t_shell_state *shell)
{
	char *result;
	t_heredoc_ctx ctx;
	int start;

	if (!content || quoted)
		return (content);
	result = gc_strdup(shell->gc, "");
	start = 0;
	ctx.result = &result;
	ctx.content = content;
	ctx.start = &start;
	ctx.shell = shell;
	ctx.i = 0;
	while (content[ctx.i])
	{
		if (content[ctx.i] == '\n')
			process_newline(&ctx);
		ctx.i++;
	}
	if (start < ctx.i)
	{
		result = gc_strjoin(shell->gc, result, process_line(content, start, ctx.i, shell));
	}
	return (result);
}

int exec_heredoc_from_content(char *content, int quoted, t_shell_state *shell)
{
	int pipefd[2];
	char *expanded;

	if (pipe(pipefd) == -1)
		return (-1);
	if (!content)
	{
		close(pipefd[1]);
		return (pipefd[0]);
	}
	expanded = expand_heredoc_content(content, quoted, shell);
	write(pipefd[1], expanded, ft_strlen(expanded));
	close(pipefd[1]);
	return (pipefd[0]);
}

char *read_line_from_stdin(t_shell_state *shell)
{
	char buffer[2];
	char *line;
	char *tmp;
	ssize_t bytes;

	line = gc_strdup(shell->gc, "");
	buffer[1] = '\0';
	while (1)
	{
		bytes = read(STDIN_FILENO, buffer, 1);
		if (bytes <= 0)
			return (NULL);
		if (buffer[0] == '\n')
			return (line);
		tmp = line;
		line = gc_strjoin(shell->gc, tmp, buffer);
	}
}
