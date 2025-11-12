/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 03:53:57 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 19:56:45 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int handle_syntax_error(char **args, int i)
{
	if ((strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0 || strcmp(args[i], "<<") == 0) && args[i + 1] == NULL)
	{
		ft_putstr_fd("minishell: syntax error near token `newline`\n",
					 STDERR_FILENO);
		return (1);
	}
	return (0);
}

static int process_operator(char **args, int *i, int *in_fd, int *out_fd)
{
	t_redir_data data;
	int res;

	data.args = args;
	data.idx = i;
	data.in_fd = in_fd;
	data.out_fd = out_fd;
	if (handle_syntax_error(args, *i))
		return (2);
	res = handle_attached_operators(&data, args[*i]);
	if (res != 2)
		return (res);
	res = handle_separated_operators(args, i, in_fd, out_fd);
	return (res);
}

int setup_redirections(char **args, int *in_fd, int *out_fd)
{
	int i;
	int res;

	*in_fd = -1;
	*out_fd = -1;
	if (!args)
		return (0);
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '<' && args[i][0] != '>')
		{
			i++;
			continue;
		}
		res = process_operator(args, &i, in_fd, out_fd);
		if (res == 0)
			continue;
		if (res == 1 || res == 2)
			return (res);
		i++;
	}
	return (0);
}
