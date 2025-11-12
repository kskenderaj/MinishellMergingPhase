/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 01:59:44 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 17:40:39 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int red_len(char *input, int i)
{
	if (!input[i])
		return (0);
	if ((input[i] == '<' && input[i + 1] == '<') || (input[i] == '>' && input[i + 1] == '>'))
	{
		return (2);
	}
	if (input[i] == '<' || input[i] == '>')
		return (1);
	return (0);
}

t_toktype red_type(const char *str, int i)
{
	if (str[i] == '<' && str[i + 1] == '<')
		return (T_HEREDOC);
	if (str[i] == '>' && str[i + 1] == '>')
		return (T_REDIR_APPEND);
	if (str[i] == '<')
		return (T_REDIR_IN);
	return (T_REDIR_OUT);
}

int handle_quote(char *input, int *i)
{
	int next;

	if (input[*i] != '\'' && input[*i] != '\"')
		return 0;
	next = scan_quote(input, *i);
	if (next < 0)
		return 1;
	*i = next;
	return 0;
}

int word_end(char *input, int i)
{
	int next;

	while (input[i] && input[i] != ' ' && input[i] != '\t' && input[i] != '|' && input[i] != '<' && input[i] != '>')
	{
		if (input[i] == '\'' || input[i] == '"')
		{
			next = scan_quote(input, i);
			if (next < 0)
				return (-1);
			i = next;
		}
		else
			i++;
	}
	return (i);
}

int handle_redir(t_token_list *lst, char *input, int *i, int rlen)
{
	int start;
	int next;
	int end;

	if (!rlen)
		return (1);
	if (add_token(lst, red_type(input, *i), input + *i, rlen) != 0)
		return (1);
	*i += rlen;
	*i = skip_spaces(input, *i);
	/* If missing target or another operator follows, don't fail here; leave parsing to detect */
	if (!input[*i] || input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
		return (0);
	if (input[*i] == '\'' || input[*i] == '"')
	{
		start = *i;
		next = scan_quote(input, *i);
		if (next < 0)
			return (1);
		if (add_token(lst, T_IDENTIFIER, input + start, next - start) != 0)
			return (1);
		*i = next;
		return (0);
	}
	start = *i;
	end = word_end(input, *i);
	if (end <= start)
		return (0);
	if (add_token(lst, T_IDENTIFIER, input + start, end - start) != 0)
		return (1);
	*i = end;
	return (0);
}
