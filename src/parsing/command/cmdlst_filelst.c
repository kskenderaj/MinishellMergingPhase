#include "minishell.h"

/* forward prototype for local helper */
static void push_file(t_file_list *lst, t_file_node *node);

t_cmd_node *create_cmdnode(void)
{
	t_cmd_node *cmdnode;

	cmdnode = gc_malloc(sizeof(*cmdnode));
	if (!cmdnode)
		return (NULL);
	cmdnode->cmd_type = CMD;
	cmdnode->cmd = NULL;
	cmdnode->files = (t_file_list *)gc_malloc(sizeof(t_file_list));
	if (!cmdnode->files)
		return NULL;
	cmdnode->files->head = NULL;
	cmdnode->files->size = 0;
	cmdnode->files->tail = NULL;
	return (cmdnode);
}

void create_filenode(char *str, int red_type, t_file_list *filelst)
{
	t_file_node *filenode;

	filenode = gc_malloc(sizeof(*filenode));
	if (!filenode)
		return;
	filenode->redir_type = red_type;
	filenode->filename = str;
	push_file(filelst, filenode);
	return;
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