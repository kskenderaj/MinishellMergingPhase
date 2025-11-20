#include "minishell.h"
#include <unistd.h>

/* forward prototype for local helper */
static void	push_file(t_file_list *lst, t_file_node *node);

t_cmd_node	*create_cmdnode(void)
{
	t_cmd_node	*cmdnode;

	cmdnode = gc_malloc(sizeof(*cmdnode));
	if (!cmdnode)
		return (NULL);
	cmdnode->cmd_type = CMD;
	cmdnode->cmd = NULL;
	cmdnode->next = NULL;
	cmdnode->files = (t_file_list *)gc_malloc(sizeof(t_file_list));
	if (!cmdnode->files)
		return (NULL);
	cmdnode->env = (t_env_list *)gc_malloc(sizeof(t_env_list));
	if (!cmdnode->env)
		return (NULL);
	init_env_lst(cmdnode->env);
	cmdnode->files->head = NULL;
	cmdnode->files->size = 0;
	cmdnode->files->tail = NULL;
	return (cmdnode);
}

void	create_filenode(char *filename, int red_type, t_file_list *filelst)
{
	t_file_node		*filenode;
	t_heredoc_info	*hdoc_info;

	filenode = gc_malloc(sizeof(*filenode));
	if (!filenode)
		return ;
	filenode->redir_type = red_type;
	filenode->heredoc_quoted = 0;
	filenode->heredoc_content = NULL;
	filenode->next = NULL;
	if (red_type == 6)
	{
		hdoc_info = process_heredoc_delimiter(filename);
		if (hdoc_info)
		{
			filenode->filename = hdoc_info->delimiter;
			filenode->heredoc_quoted = hdoc_info->quoted;
		}
		else
			filenode->filename = filename;
	}
	else
		filenode->filename = filename;
	push_file(filelst, filenode);
	return ;
}

void	push_cmd(t_cmd_list *lst, t_cmd_node *node)
{
	if (!lst || !node)
		return ;
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

void	push_file(t_file_list *lst, t_file_node *node)
{
	if (!lst || !node)
		return ;
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

// Read ALL heredocs but return the last one
static t_file_node	*read_all_heredocs_in_cmd(t_cmd_node *cmd)
{
	t_file_node	*current;
	t_file_node	*last_heredoc;

	if (!cmd || !cmd->files)
		return (NULL);
	last_heredoc = NULL;
	current = cmd->files->head;
	while (current)
	{
		if (current->redir_type == 6)  // HEREDOC
		{
			// Read this heredoc (bash reads ALL heredocs)
			if (isatty(STDIN_FILENO))
			{
				// Read the heredoc content and store it
				current->heredoc_content = read_heredoc_content(current->filename);
				// Discard the previous heredoc's content (keep only the last)
				if (last_heredoc)
					last_heredoc->heredoc_content = NULL;
			}
			last_heredoc = current;
		}
		current = current->next;
	}
	return (last_heredoc);
}

void	process_all_heredocs(t_cmd_list *cmdlst)
{
	t_cmd_node	*cmd;

	if (!cmdlst)
		return ;
	cmd = cmdlst->head;
	while (cmd)
	{
		// Read all heredocs for this command (bash behavior)
		// Only the last one will be used, but all must be read
		read_all_heredocs_in_cmd(cmd);
		cmd = cmd->next;
	}
}