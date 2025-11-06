/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 18:16:35 by jtoumani          #+#    #+#             */
/*   Updated: 2025/09/26 14:23:08 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "minishell.h"
#include "parser.h"

bool	is_boundary_char(char c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>');
}

bool	is_valid_red(char *str, int *i)
{
	while (str[*i] &&( str[*i] == ' ' || str[*i] == '\t'))
		(*i)++;
	if (str[*i] == '\0' || str[*i] == '<' || str[*i] == '>' || str[*i] == '|')
		return (false);
	return (true);
}

bool	is_red(char *str, int *i)
{
	if (str[*i] == '<' && str[*i + 1] == '<')
	{
		(*i) += 2;
		return (is_valid_red(str, i));
	}
	else if (str[*i] == '>' && str[*i + 1] == '>')
	{
		(*i) += 2;
		return (is_valid_red(str, i));
	}
	else if (str[*i] == '<')
	{
		(*i)++;
		return (is_valid_red(str, i));
	}
	else if (str[*i] == '>')
	{
		(*i)++;
		return (is_valid_red(str, i));
	}

	return (false);
}

bool	is_valid_pipe(char *str, int *i)
{
	(*i)++;
	*i = skip_spaces(str, *i);
	if (str[*i] == '\0' || str[*i] == '|')
		return (false);
	return (true);
}

bool	check_tokens(char *str, int i)
{
	if (!str)
		return (false);
	while (str[i])
	{
		i = skip_spaces(str, i);
		if (!str || str[i] == '\0')
			break ;
		if (str[i] == '|')
		{
			if (!is_valid_pipe(str, &i))
				return (false);
		}
		else if ((str[i] == '<' || str[i] == '>') && !is_red(str, &i))
			return (false);
		else if (str[i] == '\'' || str[i] == '\"')
		{
			if (!is_valid_quote(str, &i))
				return (false);
		}
		else if (!we_have_token(str, &i))
			return (false);
	}
	return (true);
}
