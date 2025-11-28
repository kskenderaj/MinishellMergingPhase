/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 13:43:20 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 13:43:33 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

static void	print_node_info(t_gc_node *node, int i)
{
	if (node->type == GC_MEM)
		printf(" [%d] MEM: %p\n", i, node->ptr);
	else if (node->type == GC_FD)
		printf(" [%d] FD: %d\n", i, node->fd);
	else if (node->type == GC_PERSISTENT)
		printf(" [%d] PERSISTENT: %p\n", i, node->ptr);
}

void	gc_print(t_gc *gc)
{
	t_gc_node	*current;
	int			i;

	if (!gc)
	{
		printf("GC is NULL\n");
		return ;
	}
	printf("GC has %zu items:\n", gc->count);
	current = gc->head;
	i = 0;
	while (current)
	{
		print_node_info(current, i);
		current = current->next;
		i++;
	}
}
