/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_separated.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 13:50:15 by kskender          #+#    #+#             */
/*   Updated: 2025/11/18 13:22:56 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	handle_output_redirection(char **args, int *i, int *out_fd)
{
	int	flags;

	if (strcmp(args[*i], ">") == 0)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (strcmp(args[*i], ">>") == 0)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		return (1);
	if (!args[*i + 1])
		return (1);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(args[*i + 1], flags, 0644);
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int	handle_input_redirection(char **args, int *i, int *in_fd)
{
	if (strcmp(args[*i], "<") == 0 && args[*i + 1])
	{
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = open(args[*i + 1], O_RDONLY);
	}
	else if (strcmp(args[*i], "<<") == 0 && args[*i + 1])
	{
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = exec_heredoc(args[*i + 1], 0);
	}
	else
		return (1);
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int	handle_separated_operators(char **args, int *i, int *in_fd, int *out_fd)
{
	if (handle_output_redirection(args, i, out_fd) == 0)
		return (0);
	if (handle_input_redirection(args, i, in_fd) == 0)
		return (0);
	return (2);
}
