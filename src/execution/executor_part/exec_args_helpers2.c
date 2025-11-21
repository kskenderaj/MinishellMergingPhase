// KLEJDI HEADER

#include "executor.h"
#include "minishell.h"

void	init_parse_state(t_parse_state *state, char **str, char delim)
{
	state->scan = *str;
	state->total = 0;
	state->first_quote = 0;
	state->all_quoted = 1;
	while (*(state->scan) == delim)
		(state->scan)++;
	state->p = state->scan;
}

void	handle_escape_seq(t_parse_state *state)
{
	if (state->p[1])
	{
		state->total++;
		state->p += 2;
	}
	else
		state->p++;
}

void	handle_quoted_segment(t_parse_state *state)
{
	char	quote_char;
	char	*start;

	quote_char = *(state->p);
	if (state->first_quote == 0)
		state->first_quote = quote_char;
	state->p++;
	start = state->p;
	while (*(state->p) && *(state->p) != quote_char)
		state->p++;
	state->total += (int)(state->p - start);
	if (*(state->p) == quote_char)
		state->p++;
}

int	calculate_word_length(t_parse_state *state, char delim)
{
	while (*(state->p) && *(state->p) != delim)
	{
		if (*(state->p) == '\\')
			handle_escape_seq(state);
		else if (*(state->p) == '\'' || *(state->p) == '"')
			handle_quoted_segment(state);
		else
		{
			state->all_quoted = 0;
			state->total++;
			state->p++;
		}
	}
	return (state->total);
}

char	*allocate_result_buffer(t_parse_state *state)
{
	char	*result;
	int		needs_marker;

	needs_marker = 0;
	if (state->all_quoted && state->first_quote != 0)
		needs_marker = 1;
	result = gc_malloc((size_t)state->total + needs_marker + 1);
	if (!result)
		return (NULL);
	if (needs_marker)
	{
		if (state->first_quote == '\'')
			result[0] = '\x01';
		else
			result[0] = '\x02';
	}
	return (result);
}
