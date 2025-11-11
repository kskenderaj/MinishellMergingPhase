/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdlst_filelst.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 01:36:05 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 04:26:19 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* forward prototype for local helper */
static void push_file(t_file_list *lst, t_file_node *node);

t_cmd_node *create_cmdnode(void)
{
	t_cmd_node *cmdnode;

	cmdnode = (t_cmd_node *)malloc(sizeof(*cmdnode));
	if (!cmdnode)
		return (NULL);
	cmdnode->cmd_type = CMD;
	cmdnode->cmd = NULL;
	cmdnode->files = (t_file_list *)malloc(sizeof(t_file_list));
	if (!cmdnode->files)
	{
		free(cmdnode);
		return (NULL);
	}
	cmdnode->files->head = NULL;
	cmdnode->files->size = 0;
	cmdnode->files->tail = NULL;
	cmdnode->next = NULL;
	cmdnode->env = NULL;
	return (cmdnode);
}

void create_filenode(char *str, int red_type, t_file_list *filelst)
{
	t_file_node *filenode;

	filenode = (t_file_node *)malloc(sizeof(*filenode));
	if (!filenode)
		return;
	filenode->redir_type = red_type;
	filenode->filename = str;
	filenode->next = NULL;
	push_file(filelst, filenode);
}

void push_cmd(t_cmd_list *lst, t_cmd_node *node)
{
	if (!lst || !node)
		return;
	if (!lst->head)
	{
		lst->head = node;
		lst->tail = node;
	}
	else
	{
		lst->tail->next = node;
		lst->tail = node;
	}
	lst->size++;
}

void push_file(t_file_list *lst, t_file_node *node)
{
	if (!lst || !node)
		return;
	if (!lst->head)
	{
		lst->head = node;
		lst->tail = node;
	}
	else
	{
		lst->tail->next = node;
		lst->tail = node;
	}
	lst->size++;
}
