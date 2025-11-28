/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd_helper3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/28 14:52:01 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

int	skip_redirection(t_token **token)
{
	if (!(*token)->next || (*token)->next->type != TK_WORD)
		return (-1);
	*token = (*token)->next;
	if (*token)
		*token = (*token)->next;
	return (0);
}

int	process_word_result(t_token *token, int ret, int *i)
{
	if (ret < 0)
		return (-1);
	if (ret == 0 && is_valid_env_assignment(token->value) && *i == 0)
		return (1);
	return (0);
}

int	process_tok_to_array(t_token_process_ctx *ctx)
{
	int	ret;

	while (ctx->token && ctx->token->type != TK_PIPE)
	{
		if (is_redirection(ctx->token->type))
		{
			if (skip_redirection(&ctx->token) < 0)
				return (-1);
			continue ;
		}
		if (ctx->token && ctx->token->type == TK_WORD)
		{
			ret = handle_word_token(ctx);
			if (process_word_result(ctx->token, ret, ctx->i) == 1)
			{
				ctx->token = ctx->token->next;
				continue ;
			}
			if (ret < 0)
				return (-1);
		}
		ctx->token = ctx->token->next;
	}
	return (0);
}
