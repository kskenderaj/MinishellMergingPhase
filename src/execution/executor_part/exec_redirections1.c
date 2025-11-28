/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirections1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:43:35 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 16:10:09 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	count_input(t_commandlist *cmd)
{
	t_file_node	*current;
	int			count;

	count = 0;
	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == INFILE || current->redir_type == HEREDOC)
			count++;
		current = current->next;
	}
	return (count);
}

int	setup_input_file_from_cmd(t_cmd_node *cmd, t_shell_state *shell)
{
	t_commandlist	tmp;

	if (!cmd || !cmd->files || !cmd->files->head)
		return (NO_REDIRECTION);
	ft_memset(&tmp, 0, sizeof(t_commandlist));
	tmp.files = (t_filelist *)cmd->files->head;
	return (setup_input_file(&tmp, shell));
}

int	setup_output_file_from_cmd(t_cmd_node *cmd, t_shell_state *shell)
{
	t_commandlist	tmp;

	if (!cmd || !cmd->files || !cmd->files->head)
		return (NO_REDIRECTION);
	ft_memset(&tmp, 0, sizeof(t_commandlist));
	tmp.files = (t_filelist *)cmd->files->head;
	return (setup_output_file(&tmp, shell));
}

static int	open_output_file(t_file_node *current, t_shell_state *shell)
{
	int	fd;
	int	flags;

	if (current->redir_type == OUTFILE)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else
		flags = O_WRONLY | O_CREAT | O_APPEND;
	fd = open(current->filename, flags, 0644);
	if (fd < 0)
	{
		perror(current->filename);
		shell->last_status = 1;
		return (-1);
	}
	close(fd);
	return (0);
}

int	open_all_output_files(t_commandlist *cmd, t_shell_state *shell)
{
	t_file_node	*current;

	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == OUTFILE
			|| current->redir_type == OUTFILE_APPEND)
		{
			if (open_output_file(current, shell) == -1)
				return (-1);
		}
		current = current->next;
	}
	return (0);
}
