/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd_helper2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:19:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"

int	handle_split_word(char **cmd_array, char *value, int *i,
		t_shell_state *shell)
{
	char	**words;
	int		j;

	words = split_on_spaces(value, shell);
	if (!words)
		return (-1);
	j = 0;
	while (words[j])
	{
		cmd_array[*i] = words[j];
		(*i)++;
		j++;
	}
	return (0);
}

int	handle_env_assignment(t_token *token, t_cmd_node *cmdnode,
		t_shell_state *shell)
{
	t_env_node	*env_node;

	env_node = gc_malloc(shell->gc, sizeof(t_env_node));
	if (!env_node)
		return (-1);
	if (!find_key(token->value, env_node))
		return (-1);
	if (!find_value(token->value, env_node))
		return (-1);
	push_env(cmdnode->env, env_node);
	return (0);
}

int	handle_word_token(t_token_process_ctx *ctx)
{
	if (*ctx->i == 0 && is_valid_env_assignment(ctx->token->value))
		return (handle_env_assignment(ctx->token, ctx->cmdnode, ctx->shell));
	if (*ctx->i == 0 && ctx->token->value && ctx->token->segment_list
		&& should_split(ctx->token->segment_list)
		&& ft_strchr(ctx->token->value, ' '))
	{
		if (handle_split_word(ctx->cmd_array, ctx->token->value, ctx->i,
				ctx->shell) < 0)
			return (-1);
	}
	else
	{
		ctx->cmd_array[*ctx->i] = ctx->token->value;
		(*ctx->i)++;
	}
	if (*ctx->i > 0 && ctx->cmd_array[0] && is_built_in(ctx->cmd_array[0]))
		ctx->cmdnode->cmd_type = BUILTIN;
	return (0);
}

static int	expand_word_token(t_token *token, t_shell_state *shell)
{
	t_segment_list	*seglst;

	seglst = gc_malloc(shell->gc, sizeof(*seglst));
	if (!seglst)
		return (-1);
	init_segment_lst(seglst);
	if (find_segment(seglst, token->value, shell))
	{
		token->segment_list = seglst;
		token->value = segments_expand(seglst, shell->env, shell->last_status,
				shell);
	}
	return (0);
}

int	process_single_token(t_token *token, int *skip_next, t_shell_state *shell)
{
	if (*skip_next)
	{
		*skip_next = 0;
		return (0);
	}
	if (token->type == TK_HEREDOC)
	{
		*skip_next = 1;
		return (0);
	}
	if (token->type == TK_WORD)
		return (expand_word_token(token, shell));
	return (0);
}
