/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/21 15:03:19 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"
#include "minishell.h"

void	process_char(char **old, t_expand_ctx *ctx, int *i)
{
	char	*new;
	int		len;

	if (ctx->seg_str[*i] == '$' && (ctx->type == SEG_DOU
			|| ctx->type == SEG_NO))
	{
		len = skip_var(ctx->seg_str + *i);
		if (len == 1)
			new = gc_strjoin(*old, gc_substr(ctx->seg_str + *i, 0, 1));
		else
			new = gc_strjoin(*old, process_dollar(ctx->seg_str, ctx->type,
						ctx->envlst, *i));
		*old = new;
		*i += (len - 1);
	}
	else
		*old = gc_strjoin(*old, gc_substr(ctx->seg_str + *i, 0, 1));
}

char	*segments_expand(t_segment_list *seglst, t_env_list *envlst,
		int last_status)
{
	t_segment	*curr;
	char		*tmp;
	char		*expansion;
	char		*new_segment;

	if (!seglst)
		return (NULL);
	tmp = gc_substr("", 0, 0);
	curr = seglst->head;
	while (curr)
	{
		expansion = expand_or_not(curr->value, curr->type, envlst, last_status);
		if (!expansion)
			return (NULL);
		new_segment = gc_strjoin(tmp, expansion);
		tmp = new_segment;
		curr = curr->next;
	}
	return (tmp);
}
