/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:39:30 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 13:42:54 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

void	gc_free(t_gc *gc, void *ptr)
{
	t_gc_node	*current;
	t_gc_node	*prev;

	if (!gc || !ptr)
		return ;
	current = gc->head;
	prev = NULL;
	while (current)
	{
		if ((current->type == GC_MEM || current->type == GC_PERSISTENT)
			&& current->ptr == ptr)
		{
			if (prev)
				prev->next = current->next;
			else
				gc->head = current->next;
			free(current->ptr);
			free(current);
			gc->count--;
			return ;
		}
		prev = current;
		current = current->next;
	}
}

void	gc_close(t_gc *gc, int fd)
{
	t_gc_node	*current;
	t_gc_node	*prev;

	if (!gc || fd < 0)
		return ;
	current = gc->head;
	prev = NULL;
	while (current)
	{
		if (current->type == GC_FD && current->fd == fd)
		{
			if (prev)
				prev->next = current->next;
			else
				gc->head = current->next;
			close(current->fd);
			free(current);
			gc->count--;
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	free_node_and_advance(t_gc_node **current, t_gc_node **prev,
		t_gc_node *next, t_gc *gc)
{
	if (*prev)
		(*prev)->next = next;
	else
		gc->head = next;
	if ((*current)->type == GC_MEM)
		free((*current)->ptr);
	else if ((*current)->type == GC_FD)
		close((*current)->fd);
	free(*current);
	gc->count--;
	*current = next;
}

void	gc_clear(t_gc *gc)
{
	t_gc_node	*current;
	t_gc_node	*next;
	t_gc_node	*prev;

	if (!gc)
		return ;
	current = gc->head;
	prev = NULL;
	while (current)
	{
		next = current->next;
		if (current->type == GC_PERSISTENT)
		{
			prev = current;
			current = next;
			continue ;
		}
		free_node_and_advance(&current, &prev, next, gc);
	}
}

void	gc_cleanup(t_gc *gc)
{
	t_gc_node	*current;
	t_gc_node	*next;

	if (!gc)
		return ;
	current = gc->head;
	while (current)
	{
		next = current->next;
		if (current->type == GC_MEM || current->type == GC_PERSISTENT)
			free(current->ptr);
		else if (current->type == GC_FD)
			close(current->fd);
		free(current);
		current = next;
	}
	free(gc);
}
