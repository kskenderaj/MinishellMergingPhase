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
	cmdnode->next = NULL;
	cmdnode->files = (t_file_list *)gc_malloc(sizeof(t_file_list));
	if (!cmdnode->files)
		return NULL;
	cmdnode->env = (t_env_list *)gc_malloc(sizeof(t_env_list));
	if (!cmdnode->env)
		return NULL;
	init_env_lst(cmdnode->env);
	cmdnode->files->head = NULL;
	cmdnode->files->size = 0;
	cmdnode->files->tail = NULL;
	return (cmdnode);
}

void create_filenode(char *filename, int red_type, t_file_list *filelst)
{
	t_file_node *filenode;
	t_heredoc_info *hdoc_info;

	filenode = gc_malloc(sizeof(*filenode));
	if (!filenode)
		return;
	filenode->redir_type = red_type;
	filenode->heredoc_quoted = 0;
	filenode->heredoc_content = NULL;
	if (red_type == 6)
	{
		hdoc_info = process_heredoc_delimiter(filename);
		if (hdoc_info)
		{
			filenode->filename = hdoc_info->delimiter;
			filenode->heredoc_quoted = hdoc_info->quoted;
			if (isatty(STDIN_FILENO))
				filenode->heredoc_content = read_heredoc_content(hdoc_info->delimiter);
		}
		else
			filenode->filename = filename;
	}
	else
		filenode->filename = filename;
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