/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_separated.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 13:50:15 by kskender          #+#    #+#             */
/*   Updated: 2025/11/11 20:29:48 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <string.h>

static int print_file_error(const char *name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)name, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putendl_fd((char *)strerror(errno), STDERR_FILENO);
	return (1);
}

int handle_output_redirection(char **args, int *i, int *out_fd)
{
	int flags;

	if (strcmp(args[*i], ">") == 0)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (strcmp(args[*i], ">>") == 0)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		return (1);
	if (!args[*i + 1])
		return (2);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(args[*i + 1], flags, 0644);
	if (*out_fd == -1)
		return (print_file_error(args[*i + 1]));
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int handle_input_redirection(char **args, int *i, int *in_fd)
{
	if (strcmp(args[*i], "<") == 0 && args[*i + 1])
	{
		if (!strcmp(args[*i + 1], ">") || !strcmp(args[*i + 1], "<") || !strcmp(args[*i + 1], ">>") || !strcmp(args[*i + 1], "<<"))
			return (2);
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = open(args[*i + 1], O_RDONLY);
		if (*in_fd == -1)
			return (print_file_error(args[*i + 1]));
	}
	else if (strcmp(args[*i], "<<") == 0 && args[*i + 1])
	{
		if (!strcmp(args[*i + 1], ">") || !strcmp(args[*i + 1], "<") || !strcmp(args[*i + 1], ">>") || !strcmp(args[*i + 1], "<<"))
			return (2);
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = exec_heredoc(args[*i + 1]);
		if (*in_fd == -1)
			return (1);
	}
	else
		return (2);
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int handle_separated_operators(char **args, int *i, int *in_fd, int *out_fd)
{
	if (handle_output_redirection(args, i, out_fd) == 0)
		return (0);
	if (handle_input_redirection(args, i, in_fd) == 0)
		return (0);
	return (2);
}
