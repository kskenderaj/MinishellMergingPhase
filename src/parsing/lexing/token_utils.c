/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:37:54 by jtoumani          #+#    #+#             */
/*   Updated: 2025/09/26 12:35:14 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "minishell.h"
#include "parser.h"

int	skip_spaces(char *str, int i)
{
	while (str[i] && (str[i] == ' ' || str[i] == '\t'))
		i++;
	return (i);
}

bool	we_have_token(const char *str, int *i)
{
	int	start;
	int	next;

	start = *i;
	while (str[*i] && str[*i] != ' ' && str[*i] != '\t' && str[*i] != '<'
		&& str[*i] != '>' && str[*i] != '|')
	{
		if (str[*i] == '\'' || str[*i] == '\"')
		{
			next = scan_quote(str, *i);
			if (next < 0)
				return (false);
			*i = next;
		}
		else
			(*i)++;
	}
	return (*i > start);
}

int	scan_word(char *str, int i)
{
	int	j;

	j = i;
	if (!we_have_token(str, &j))
		return (-1);
	return (j);
}

int	scan_quote(const char *str, int i)
{
	char	start;
	int		end;

	if (!str || !str[i] || (str[i] != '\'' && str[i] != '\"'))
		return (i);
	start = str[i++];
	end = i;
	while (str[end] && str[end] != start)
		end++;
	if (!str[end])
		return (-1);
	return (end + 1);
}

bool	is_valid_quote(char *str, int *i)
{
	int	next;

	next = scan_quote(str, *i);
	if (next < 0)
		return (false);
	*i = next;
	return (true);
}
