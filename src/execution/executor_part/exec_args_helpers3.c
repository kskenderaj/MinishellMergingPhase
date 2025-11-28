/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_args_helpers3.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 17:30:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/26 13:54:18 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

void	copy_escape_char(char **p, char *out, int *idx, int marker)
{
	if ((*p)[1])
	{
		out[*idx + marker] = (*p)[1];
		(*idx)++;
		*p += 2;
	}
	else
		(*p)++;
}

void	copy_quoted_content(char **p, char *out, int *idx, int marker)
{
	char	quote_char;

	quote_char = **p;
	(*p)++;
	while (**p && **p != quote_char)
	{
		out[*idx + marker] = **p;
		(*idx)++;
		(*p)++;
	}
	if (**p == quote_char)
		(*p)++;
}

void	fill_result_buffer(t_parse_state *state, char *result, char delim)
{
	int	idx;
	int	marker;

	marker = 0;
	if (state->all_quoted && state->first_quote != 0)
		marker = 1;
	idx = 0;
	state->p = state->scan;
	while (*(state->p) && *(state->p) != delim)
	{
		if (*(state->p) == '\\')
			copy_escape_char(&state->p, result, &idx, marker);
		else if (*(state->p) == '\'' || *(state->p) == '"')
			copy_quoted_content(&state->p, result, &idx, marker);
		else
		{
			result[idx + marker] = *(state->p);
			idx++;
			state->p++;
		}
	}
	result[idx + marker] = '\0';
}
