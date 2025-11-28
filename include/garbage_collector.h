/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:02:10 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 13:45:55 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GARBAGE_COLLECTOR_H
# define GARBAGE_COLLECTOR_H
# include "libft.h"
# include <fcntl.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

typedef struct s_gc_collector	t_gc_collector;

typedef enum e_gc_type
{
	GC_MEM,
	GC_FD,
	GC_PERSISTENT
}								t_gc_type;

typedef struct s_gc_node
{
	void						*ptr;
	int							fd;
	t_gc_type					type;
	struct s_gc_node			*next;
}								t_gc_node;

typedef struct s_gc
{
	t_gc_node					*head;
	size_t						count;
}								t_gc;

// Core GC initialization and cleanup
t_gc							*gc_init(void);
void							gc_clear(t_gc *gc);
void							gc_cleanup(t_gc *gc);
int								gc_add_node(t_gc *gc, void *ptr, int fd,
									t_gc_type type);

// Basic memory management
void							*gc_malloc(t_gc *gc, size_t size);
void							*gc_malloc_persistent(t_gc *gc, size_t size);
void							*gc_calloc(t_gc *gc, size_t count, size_t size);

// Basic file descriptor management
void							gc_register_fd(t_gc *gc, int fd);

// Individual cleanup
void							gc_free(t_gc *gc, void *ptr);
void							gc_close(t_gc *gc, int fd);

// Utility functions
void							gc_print(t_gc *gc);
size_t							gc_count(t_gc *gc);

// String utilities
char							*gc_strdup(t_gc *gc, const char *s);
char							*gc_strdup_persistent(t_gc *gc, const char *s);
char							*gc_strndup(t_gc *gc, const char *s, size_t n);
char							**gc_split(t_gc *gc, const char *s, char c);
char							*gc_substr(t_gc *gc, const char *s,
									unsigned int start, size_t len);
char							*gc_itoa(t_gc *gc, int n);
char							*gc_strjoin(t_gc *gc, const char *s1,
									const char *s2);

// File descriptor utilities
int								gc_open(t_gc *gc, const char *path, int flags,
									...);
int								gc_pipe(t_gc *gc, int pipefd[2]);
int								gc_dup(t_gc *gc, int oldfd);
int								gc_dup2(t_gc *gc, int oldfd, int newfd);

#endif