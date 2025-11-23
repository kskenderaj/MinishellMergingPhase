/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd_helper3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/23 16:36:39 by kskender         ###   ########.fr       */
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

int	process_tokens_to_array(t_token *token, t_cmd_node *cmdnode,
		char **cmd_array, int *i, t_shell_state *shell)

{
	int	ret;

	while (token && token->type != TK_PIPE)
	{
		if (is_redirection(token->type))
		{
			if (skip_redirection(&token) < 0)
				return (-1);
			continue ;
		}
		if (token && token->type == TK_WORD)
		{
			ret = handle_word_token(token, cmdnode, cmd_array, i, shell);
			if (process_word_result(token, ret, i) == 1)
			{
				token = token->next;
				continue ;
			}
			if (ret < 0)
				return (-1);
		}
		token = token->next;
	}
	return (0);
}
