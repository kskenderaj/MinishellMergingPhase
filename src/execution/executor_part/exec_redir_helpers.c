/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 03:53:57 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 03:54:02 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* implementation intentionally moved to exec_redir_separated.c */

/* Canonical setup_redirections implementation */
int setup_redirections(char **args, int *in_fd, int *out_fd)
{
	int i;
	int res;
	char *tok;
	t_redir_data data;

	*in_fd = -1;
	*out_fd = -1;
	i = 0;
	while (args[i])
	{
		tok = args[i];
		data.args = args;
		data.idx = &i;
		data.in_fd = in_fd;
		data.out_fd = out_fd;
		if ((strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0 || strcmp(tok, "<") == 0 || strcmp(tok, "<<") == 0) && args[i + 1] == NULL)
		{
			ft_putstr_fd("minishell: syntax error near token `newline`\n",
						 STDERR_FILENO);
			return (2);
		}
		res = handle_attached_operators(&data, tok);
		if (res != 2)
		{
			if (res != 0)
				return (2);
			continue;
		}
		res = handle_separated_operators(args, &i, in_fd, out_fd);
		if (res != 2)
		{
			if (res == 1)
				return (1); /* file open error */
			if (res != 0)
				return (2); /* syntax */
			continue;
		}
		i++;
	}
	if ((*in_fd != -1 && *in_fd < 0) || (*out_fd != -1 && *out_fd < 0))
		return (1);
	return (0);
}
