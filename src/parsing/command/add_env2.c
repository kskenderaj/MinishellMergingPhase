/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_env2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/21 14:46:55 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

char	*get_expand(char *seg_str, int i, int last_status, t_env_list *envlst)
{
	if (seg_str[i + 1] == '?')
		return (gc_itoa(last_status));
	else
		return (expand_env(seg_str + i, envlst));
}

void	free_env_node(t_env_node *node)
{
	if (node->key)
		free(node->key);
	if (node->value)
		free(node->value);
	free(node);
}

void	remove_from_env_list(t_env_list *envlst, const char *key)
{
	t_env_node	*prev;
	t_env_node	*curr;

	if (!envlst || !key)
		return ;
	prev = NULL;
	curr = envlst->head;
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else
				envlst->head = curr->next;
			if (curr == envlst->tail)
				envlst->tail = prev;
			free_env_node(curr);
			envlst->size--;
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

void	free_env_list(t_env_list *env)
{
	t_env_node	*current;
	t_env_node	*next;

	if (!env)
		return ;
	current = env->head;
	while (current)
	{
		next = current->next;
		if (current->key)
			free(current->key);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
	if (env->pid)
		free(env->pid);
	free(env);
}
