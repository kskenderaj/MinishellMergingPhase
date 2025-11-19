/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 18:16:35 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/19 14:05:07 by kskender         ###   ########.fr       */
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
	*i = skip_spaces(str, *i);
	if (str[*i] == '\0') // nothing after redir
		return (false);
	if (str[*i] == '|') // pipe cannot follow redir
		return (false);
	// DO NOT reject < or > here.
	// bash allows "> > out", "<< >> limiter", "< > file"
	return (true);
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
		if (str[i] == '\0')
			break ;
		if (str[i] == '|')
		{
			if (!is_valid_pipe(str, &i))
				return (false);
		}
		else if ((str[i] == '<' || str[i] == '>'))
		{
			if (!is_valid_red(str, &i))
				return (false);
		}
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
