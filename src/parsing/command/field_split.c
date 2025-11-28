/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   field_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:18:35 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"

int	count_words(char *str)
{
	int	count;
	int	in_word;
	int	i;

	count = 0;
	in_word = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t')
			in_word = 0;
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		i++;
	}
	return (count);
}

char	*extract_word(char *str, int *pos, t_shell_state *shell)
{
	int		start;
	int		len;
	char	*word;

	while (str[*pos] && (str[*pos] == ' ' || str[*pos] == '\t'))
		(*pos)++;
	start = *pos;
	while (str[*pos] && str[*pos] != ' ' && str[*pos] != '\t')
		(*pos)++;
	len = *pos - start;
	if (len == 0)
		return (NULL);
	word = gc_substr(shell->gc, str, start, len);
	return (word);
}

char	**split_on_spaces(char *str, t_shell_state *shell)
{
	char	**result;
	int		word_count;
	int		i;
	int		pos;

	if (!str)
		return (NULL);
	word_count = count_words(str);
	result = gc_malloc(shell->gc, sizeof(char *) * (word_count + 1));
	if (!result)
		return (NULL);
	i = 0;
	pos = 0;
	while (i < word_count)
	{
		result[i] = extract_word(str, &pos, shell);
		if (!result[i])
			return (NULL);
		i++;
	}
	result[i] = NULL;
	return (result);
}

int	should_split(t_segment_list *seglst)
{
	t_segment	*seg;
	int			has_unquoted;

	if (!seglst || seglst->size == 0)
		return (0);
	has_unquoted = 0;
	seg = seglst->head;
	while (seg)
	{
		if (seg->type == SEG_NO)
		{
			has_unquoted = 1;
			break ;
		}
		seg = seg->next;
	}
	return (has_unquoted && seglst->size == 1);
}
