/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd_helper4.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 16:20:52 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/28 13:24:29 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"

int	count_args(t_token *token)
{
	int	i;

	i = 0;
	while (token && token->type != TK_PIPE)
	{
		if (token->type == TK_WORD)
		{
			if (token->segment_list && should_split(token->segment_list)
				&& token->value && ft_strchr(token->value, ' '))
				i += count_words(token->value);
			else
				i++;
		}
		else if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != TK_WORD)
				return (-1);
			token = token->next;
		}
		token = token->next;
	}
	return (i);
}

t_token	*skip_to_next_pipe(t_token *token)
{
	while (token && token->type != TK_PIPE)
		token = token->next;
	if (token && token->type == TK_PIPE)
		token = token->next;
	return (token);
}
