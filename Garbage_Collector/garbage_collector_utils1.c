/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector_utils1.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 21:03:21 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:45:51 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

int	gc_open(t_gc *gc, const char *pathname, int flags, ...)
{
	int		fd;
	mode_t	mode;
	va_list	args;

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
	gc_register_fd(gc, fd);
	return (fd);
}

int	gc_pipe(t_gc *gc, int pipefd[2])
{
	if (pipe(pipefd) == -1)
		return (-1);
	gc_register_fd(gc, pipefd[0]);
	gc_register_fd(gc, pipefd[1]);
	return (0);
}

int	gc_dup(t_gc *gc, int oldfd)
{
	int	newfd;

	newfd = dup(oldfd);
	if (newfd < 0)
		return (-1);
	gc_register_fd(gc, newfd);
	return (newfd);
}

int	gc_dup2(t_gc *gc, int oldfd, int newfd)
{
	int	result;

	result = dup2(oldfd, newfd);
	if (result < 0)
		return (-1);
	if (result != newfd)
		gc_register_fd(gc, result);
	return (result);
}

void	gc_register_fd(t_gc *gc, int fd)
{
	if (fd < 0)
		return ;
	gc_add_node(gc, NULL, fd, GC_FD);
}
