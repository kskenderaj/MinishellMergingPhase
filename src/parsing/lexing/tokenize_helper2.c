/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:37:54 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/26 18:17:45 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int handle_quoted_redir_file(t_token_list *lst, char *input, int *i,
							 t_shell_state *shell)
{
	int start;
	int next;
	t_tokenize_ctx ctx;

	start = *i;
	next = scan_quote(input, *i);
	if (next < 0)
		return (1);
	ctx.lst = lst;
	ctx.input = input + start;
	ctx.i = i;
	ctx.shell = shell;
	if (add_token(&ctx, TK_WORD, next - start))
		return (1);
	*i = next;
	return (0);
}

int add_redir_filename(t_token_list *lst, char *input, int *i,
					   t_shell_state *shell)
{
	int start;
	int end;
	t_tokenize_ctx ctx;

	start = *i;
	end = word_end(input, *i);
	if (end <= start)
		return (1);
	ctx.lst = lst;
	ctx.input = input + start;
	ctx.i = i;
	ctx.shell = shell;
	if (add_token(&ctx, TK_WORD, end - start) != 0)
		return (1);
	*i = end;
	return (0);
}
