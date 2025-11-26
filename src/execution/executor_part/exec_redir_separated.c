/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_separated.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 13:50:15 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 16:09:15 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int handle_output_redirection(char **args, int *i, int *out_fd)
{
	int flags;
	int fd;

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
	fd = open(args[*i + 1], flags, 0644);
	if (fd < 0)
	{
		perror(args[*i + 1]);
		return (1);
	}
	*out_fd = fd;
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int handle_input_redirection(char **args, int *i, int *in_fd,
							 t_shell_state *shell)
{
	int fd;

	if (strcmp(args[*i], "<") == 0 && args[*i + 1])
	{
		if (*in_fd != -1)
			close(*in_fd);
		fd = open(args[*i + 1], O_RDONLY);
		if (fd < 0)
			return (perror(args[*i + 1]), 1);
		*in_fd = fd;
	}
	else if (strcmp(args[*i], "<<") == 0 && args[*i + 1])
	{
		if (*in_fd != -1)
			close(*in_fd);
		*in_fd = exec_heredoc(args[*i + 1], 0, shell);
		if (*in_fd < 0)
			return (1);
	}
	else
		return (1);
	args[*i] = NULL;
	args[*i + 1] = NULL;
	shift_left_by(args, *i, 2);
	return (0);
}

int handle_separated_operators(t_redir_data *data, t_shell_state *shell)
{
	if (handle_output_redirection(data->args, data->idx, data->out_fd) == 0)
		return (0);
	if (handle_input_redirection(data->args, data->idx, data->in_fd,
								 shell) == 0)
		return (0);
	return (2);
}
