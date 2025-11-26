/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:37:54 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/26 18:17:36 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "garbage_collector.h"
#include "parser.h"

t_token *create_token(t_toktype type, char *val, t_shell_state *shell)
{
	t_token *t;

	t = gc_malloc(shell->gc, sizeof(*t));
	if (!t)
		return (NULL);
	t->type = type;
	t->value = val;
	t->next = NULL;
	return (t);
}

int push_token(t_token_list *lst, t_token *token)
{
	if (!lst || !token)
		return (1);
	if (!lst->head)
	{
		lst->head = token;
		lst->tail = token;
	}
	else
	{
		lst->tail->next = token;
		lst->tail = token;
	}
	lst->size++;
	return (0);
}

int add_token(t_tokenize_ctx *ctx, t_toktype type, int len)
{
	char *copy;
	t_token *tok;

	if (!ctx->lst || !ctx->input || len <= 0)
		return (1);
	copy = gc_substr(ctx->shell->gc, ctx->input, 0, len);
	if (!copy)
		return (1);
	tok = create_token(type, copy, ctx->shell);
	if (!tok)
		return (1);
	return (push_token(ctx->lst, tok));
}

int handle_word(t_token_list *lst, char *input, int *i, t_shell_state *shell)
{
	int next;
	int start;
	t_tokenize_ctx ctx;

	start = *i;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t' && input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			next = scan_quote(input, *i);
			if (next < 0)
				return (1);
			*i = next;
		}
		else
			(*i)++;
	}
	if (*i == start)
		return (1);
	ctx.lst = lst;
	ctx.input = input + start;
	ctx.i = i;
	ctx.shell = shell;
	return (add_token(&ctx, TK_WORD, *i - start));
}

int tokenize(t_token_list *lst, char *input, t_shell_state *shell)
{
	return (tokenize_main(lst, input, shell));
}
