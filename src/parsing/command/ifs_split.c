/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ifs_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:35:17 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "executor.h"

int	is_ifs_char(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

void	process_ifs_char(char *result, int *j, int *prev_was_space)
{
	if (!(*prev_was_space))
	{
		result[(*j)++] = ' ';
		*prev_was_space = 1;
	}
}

char	*ifs_field_split(char *str, t_shell_state *shell)
{
	char	*result;
	int		i;
	int		j;
	int		prev_was_space;

	if (!str)
		return (NULL);
	result = gc_malloc(shell->gc, ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	i = -1;
	j = 0;
	prev_was_space = 0;
	while (str[++i])
	{
		if (is_ifs_char(str[i]))
			process_ifs_char(result, &j, &prev_was_space);
		else
		{
			result[j++] = str[i];
			prev_was_space = 0;
		}
	}
	result[j] = '\0';
	return (result);
}