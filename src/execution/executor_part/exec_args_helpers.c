/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_args_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:14:26 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/21 17:31:17 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static char	*get_next_word(char **str, char delim)
{
	t_parse_state	state;
	char			*result;

	init_parse_state(&state, str, delim);
	if (*(state.scan) == '\0')
		return (NULL);
	if (calculate_word_length(&state, delim) == 0)
	{
		*str = state.scan;
		return (NULL);
	}
	result = allocate_result_buffer(&state);
	if (!result)
		return (NULL);
	fill_result_buffer(&state, result, delim);
	*str = state.p;
	return (result);
}

/* Simple argument splitter that preserves tokens returned by the tokenizer.
 * Previously this code removed marker-only tokens; doing so discards valid
 * empty quoted arguments ("" and ''), so we pass tokens through verbatim.
 */
void	split_args(char *input, char **args, int max_args)
{
	int		i;
	char	*ptr;
	char	*w;

	i = 0;
	ptr = input;
	while (i < max_args - 1)
	{
		w = get_next_word(&ptr, ' ');
		if (!w)
			break ;
		args[i++] = w;
	}
	args[i] = NULL;
}

void	shift_left_by(char **args, int start, int by)
{
	int	j;

	j = start;
	while (args[j + by])
	{
		args[j] = args[j + by];
		j++;
	}
	while (by > 0)
	{
		args[j] = NULL;
		j++;
		by--;
	}
}
