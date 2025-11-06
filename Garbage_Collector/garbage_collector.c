/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:08:39 by kskender          #+#    #+#             */
/*   Updated: 2025/11/04 22:17:07 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

t_gc *get_gc(void)
{
	static t_gc gc = {0};

	return (&gc);
}

// initializing gc
t_gc *gc_init(void)
{
	t_gc *gc;

	gc = malloc(sizeof(t_gc));
	if (!gc)
		return (NULL);
	gc->head = NULL;
	gc->count = 0;
	return (gc);
}

int gc_add_node(void *ptr, int fd, t_gc_type type)
{
	t_gc *gc;
	t_gc_node *new_node;

	gc = get_gc();
	new_node = malloc(sizeof(t_gc_node));
	if (!new_node)
		return (0);
	new_node->ptr = ptr;
	new_node->fd = fd;
	new_node->type = type;
	new_node->next = gc->head;
	gc->head = new_node;
	gc->count++;
	return (1);
}

// memory allocations Basics
void *gc_malloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	if (!gc_add_node(ptr, -1, GC_MEM))
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}
void *gc_calloc(size_t count, size_t size)
{
	void *ptr;

	ptr = calloc(count, size);
	if (!ptr)
		return (NULL);
	if (!gc_add_node(ptr, -1, GC_MEM))
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}
