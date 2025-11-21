/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ifs_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/21 12:21:22 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

/*
** ifs_field_split: Performs IFS field splitting on a string
** - Collapses consecutive whitespace to SINGLE space
** - PRESERVES leading/trailing spaces as field delimiters
** - Used only for unquoted variable expansions when adjacent to other segments
**
** Example: "  A  B  " -> " A B " (single spaces, but keeps boundary spaces)
*/
char	*ifs_field_split(char *str)
{
	char	*result;
	int		i;
	int		j;
	int		prev_was_space;

	if (!str)
		return (NULL);
	result = gc_malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	prev_was_space = 0;
	while (str[i])
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			if (!prev_was_space)
			{
				result[j++] = ' ';
				prev_was_space = 1;
			}
			i++;
		}
		else
		{
			result[j++] = str[i++];
			prev_was_space = 0;
		}
	}
	result[j] = '\0';
	return (result);
}
