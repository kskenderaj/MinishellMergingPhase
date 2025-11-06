/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector_utils1.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 21:03:21 by kskender          #+#    #+#             */
/*   Updated: 2025/10/14 23:08:14 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

// File descriptor utilities/helpers
int gc_open(const char *pathname, int flags, ...)
{
	int fd;
	mode_t mode;
	va_list args;

	if (flags & O_CREAT)
	{
		va_start(args, flags);
		mode = (mode_t)va_arg(args, int);
		va_end(args);
		fd = open(pathname, flags, mode);
	}
	else
		fd = open(pathname, flags);
	if (fd < 0)
		return (-1);
	gc_register_fd(fd);
	return (fd);
}

int gc_pipe(int pipefd[2])
{
	if (pipe(pipefd) == -1)
		return (-1);
	gc_register_fd(pipefd[0]);
	gc_register_fd(pipefd[1]);
	return (0);
}

int gc_dup(int oldfd)
{
	int newfd;

	newfd = dup(oldfd);
	if (newfd < 0)
		return (-1);
	gc_register_fd(newfd);
	return (newfd);
}

int gc_dup2(int oldfd, int newfd)
{
	int result;

	result = dup2(oldfd, newfd);
	if (result < 0)
		return (-1);
	if (result != newfd)
		gc_register_fd(result);
	return (result);
}

void gc_register_fd(int fd)
{
	if (fd < 0)
		return;
	gc_add_node(NULL, fd, GC_FD);
}
