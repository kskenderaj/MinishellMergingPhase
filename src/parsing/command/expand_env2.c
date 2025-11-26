/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/26 16:14:23 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "garbage_collector.h"
#include "minishell.h"

void process_char(char **old, t_expand_ctx *ctx, int *i,
				  t_shell_state *shell)
{
	char *new;
	int len;

	if (ctx->seg_str[*i] == '$' && (ctx->type == SEG_DOU || ctx->type == SEG_NO))
	{
		len = skip_var(ctx->seg_str + *i);
		if (len == 1)
			new = gc_strjoin(shell->gc, *old, gc_substr(shell->gc, ctx->seg_str + *i, 0, 1));
		else
			new = gc_strjoin(shell->gc, *old, process_dollar(ctx, *i, shell));
		*old = new;
		*i += (len - 1);
	}
	else
		*old = gc_strjoin(shell->gc, *old, gc_substr(shell->gc, ctx->seg_str + *i, 0, 1));
}

char *segments_expand(t_segment_list *seglst, t_env_list *envlst,
					  int last_status, t_shell_state *shell)
{
	t_segment *curr;
	char *tmp;
	char *expansion;
	char *new_segment;
	t_expand_ctx ctx;

	if (!seglst)
		return (NULL);
	tmp = gc_substr(shell->gc, "", 0, 0);
	curr = seglst->head;
	while (curr)
	{
		ctx.seg_str = curr->value;
		ctx.type = curr->type;
		ctx.envlst = envlst;
		expansion = expand_or_not(&ctx, last_status, shell);
		if (!expansion)
			return (NULL);
		new_segment = gc_strjoin(shell->gc, tmp, expansion);
		tmp = new_segment;
		curr = curr->next;
	}
	return (tmp);
}
