/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_basics.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 14:53:50 by kskender          #+#    #+#             */
/*   Updated: 2025/10/17 16:11:24 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void dup_and_or_close(int *prev_in_out, int *new_in_out)
{
	if (prev_in_out[0] != NO_REDIRECTION)
	{
		gc_dup2(prev_in_out[0], STDIN_FILENO);
		gc_close(prev_in_out[0]);
		prev_in_out[0] = NO_REDIRECTION;
	}
	if (new_in_out[1] != NO_REDIRECTION)
	{
		gc_dup2(new_in_out[1], STDOUT_FILENO);
		gc_close(new_in_out[1]);
		new_in_out[1] = NO_REDIRECTION;
	}
	if (prev_in_out[1] != NO_REDIRECTION)
	{
		gc_close(prev_in_out[1]);
		prev_in_out[1] = NO_REDIRECTION;
	}
	if (new_in_out[0] != NO_REDIRECTION)
	{
		gc_close(new_in_out[0]);
		new_in_out[0] = NO_REDIRECTION;
	}
}

int infile_redirector(t_file_node *file_node)
{
	int fd;
	fd = gc_open(file_node->filename, O_RDONLY);
	if (fd < 0)
		return (perror(file_node->filename), -1);
	if (gc_dup2(fd, STDIN_FILENO) < 0)
	{
		gc_close(fd);
		return (perror("dup2"), -1);
	}
	gc_close(fd);
	return (0);
}

int outfile_redirector(t_file_node *file_node)
{
	int fd;
	fd = gc_open(file_node->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (perror(file_node->filename), -1);
	if (gc_dup2(fd, STDOUT_FILENO) < 0)
	{
		gc_close(fd);
		return (perror("dup2"), -1);
	}
	gc_close(fd);
	return (0);
}

int append_redirector(t_file_node *file_node)
{
	int fd;
	fd = gc_open(file_node->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		return (perror(file_node->filename), -1);
	if (gc_dup2(fd, STDOUT_FILENO) < 0)
	{
		gc_close(fd);
		return (perror("dup2"), -1);
	}
	gc_close(fd);
	return (0);
}

int pipe_handler(t_cmd_node *command)
{
	int pipefd[2];

	if (command->next)
	{
		if (gc_pipe(pipefd) < 0)
			return (perror("pipe"), -1);
	}
	return (0);
}
