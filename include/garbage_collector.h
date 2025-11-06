/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:02:10 by kskender          #+#    #+#             */
/*   Updated: 2025/11/04 21:22:34 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

typedef struct s_gc_collector t_gc_collector;

#include "libft.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

typedef enum e_gc_type
{
	GC_MEM,
	GC_FD
} t_gc_type;

typedef struct s_gc_node
{
	void *ptr;
	int fd;
	t_gc_type type;
	struct s_gc_node *next;
} t_gc_node;

typedef struct s_gc
{
	t_gc_node *head;
	size_t count;
} t_gc;

// Global GC access
t_gc *get_gc(void);
// Core GC initialization and cleanup
t_gc *gc_init(void);
void gc_cleanup(void);
int gc_add_node(void *ptr, int fd, t_gc_type type);

// Basic memory management
void *gc_malloc(size_t size);
void *gc_calloc(size_t count, size_t size);

// Basic file descriptor management
void gc_register_fd(int fd);

// Individual cleanup
void gc_free(void *ptr);
void gc_close(int fd);

// Utility functions
void gc_print(void);
size_t gc_count(void);

// String utilities
char *gc_strdup(const char *s);
char *gc_strndup(const char *s, size_t n);
char **gc_split(const char *s, char c);
char *gc_substr(const char *s, unsigned int start, size_t len);
char *gc_itoa(int n);
char *gc_strjoin(const char *s1, const char *s2);

// File descriptor utilities
int gc_open(const char *path, int flags, ...);
int gc_pipe(int pipefd[2]);
int gc_dup(int oldfd);
int gc_dup2(int oldfd, int newfd);

#endif
