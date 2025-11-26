/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 14:08:07 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 18:17:49 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int red_len(char *input, int i)
{
	if (!input[i])
		return (0);
	if ((input[i] == '<' && input[i + 1] == '<') || (input[i] == '>' && input[i + 1] == '>'))
	{
		return (2);
	}
	if (input[i] == '<' || input[i] == '>')
		return (1);
	return (0);
}

t_toktype red_type(const char *str, int i)
{
	if (str[i] == '<' && str[i + 1] == '<')
		return (TK_HEREDOC);
	if (str[i] == '>' && str[i + 1] == '>')
		return (TK_APPEND);
	if (str[i] == '<')
		return (TK_INFILE);
	return (TK_OUTFILE);
}

int handle_quote(char *input, int *i)
{
	int next;

	if (input[*i] != '\'' && input[*i] != '\"')
		return (0);
	next = scan_quote(input, *i);
	if (next < 0)
		return (1);
	*i = next;
	return (0);
}

int word_end(char *input, int i)
{
	int next;

	while (input[i] && input[i] != ' ' && input[i] != '\t' && input[i] != '|' && input[i] != '<' && input[i] != '>')
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			next = scan_quote(input, i);
			if (next < 0)
				return (-1);
			i = next;
		}
		else
			i++;
	}
	return (i);
}

int handle_redir(t_tokenize_ctx *ctx, int redir_len)
{
	if (!redir_len)
		return (1);
	if (add_token(ctx, red_type(ctx->input, *ctx->i), redir_len) != 0)
		return (1);
	*ctx->i += redir_len;
	*ctx->i = skip_spaces(ctx->input, *ctx->i);
	if (!ctx->input[*ctx->i] || ctx->input[*ctx->i] == '|' || ctx->input[*ctx->i] == '<' || ctx->input[*ctx->i] == '>')
		return (1);
	return (add_redir_filename(ctx->lst, ctx->input, ctx->i, ctx->shell));
}
