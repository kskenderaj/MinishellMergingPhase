/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_token.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:34:43 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "executor.h"

int	no_quote_segment(t_segment_list *lst, char *str, int *i, t_shell_state *shell)
{
	int			start;
	t_segment	*segment;

	start = *i;
	while (str[*i] && str[*i] != '\'' && str[*i] != '\"')
		(*i)++;
	if (*i > start)
	{
		segment = create_segment(str + start, (*i - start), SEG_NO, shell);
		if (!segment || !push_segment(lst, segment))
			return (0);
	}
	return (1);
}

int	quote_segment(t_segment_list *lst, char *str, int *i, t_shell_state *shell)
{
	char		q;
	int			start;
	t_segment	*segment;
	t_seg_type	type;

	if (!str[*i] || (str[*i] != '\'' && str[*i] != '\"'))
		return (1);
	q = str[*i];
	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != q)
		(*i)++;
	if (!str[*i])
		return (0);
	if (q == '\'')
		type = SEG_SING;
	else
		type = SEG_DOU;
	segment = create_segment(str + start, *i - start, type, shell);
	if (!segment || !push_segment(lst, segment))
		return (0);
	(*i)++;
	return (1);
}

int	find_segment(t_segment_list *lst, char *str, t_shell_state *shell)
{
	int	i;

	i = 0;
	if (!str || !lst)
		return (0);
	while (str[i])
	{
		if (!no_quote_segment(lst, str, &i, shell))
			return (0);
		if (!str[i])
			break ;
		if (!quote_segment(lst, str, &i, shell))
			return (0);
	}
	return (1);
}

t_segment	*create_segment(char *start, int len, t_seg_type type, t_shell_state *shell)
{
	t_segment	*segment;

	segment = gc_malloc(shell->gc, sizeof(*segment));
	if (!segment)
		return (NULL);
	segment->value = gc_substr(shell->gc, start, 0, (size_t)len);
	if (!segment->value)
		return (NULL);
	segment->type = type;
	segment->next = NULL;
	return (segment);
}

int	push_segment(t_segment_list *lst, t_segment *segment)
{
	if (!lst || !segment)
		return (0);
	if (!lst->head)
	{
		lst->head = segment;
		lst->tail = segment;
	}
	else
	{
		lst->tail->next = segment;
		lst->tail = segment;
	}
	lst->size++;
	return (1);
}
