/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_outfile.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:28:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 16:09:02 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Counts outfile/append redirections
int count_outfile(t_commandlist *cmd)
{
	int count;
	t_filelist *cur;

	count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == OUTFILE || cur->type == OUTFILE_APPEND)
			count++;
		cur = cur->next;
	}
	return (count);
}

// Finds the last outfile/append
static t_filelist *find_last_outfile(t_commandlist *cmd, int outfile_count)
{
	int cur_count;
	t_filelist *cur;

	cur_count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == OUTFILE || cur->type == OUTFILE_APPEND)
		{
			cur_count++;
			if (cur_count == outfile_count)
				return (cur);
		}
		cur = cur->next;
	}
	return (NULL);
}

// Opens the last outfile/append and returns fd
int setup_outfile_fd(t_commandlist *cmd, t_shell_state *shell)
{
	int outfile_count;
	t_filelist *last_outfile;
	int fd;

	(void)shell;
	outfile_count = count_outfile(cmd);
	if (outfile_count == 0)
		return (-1);
	last_outfile = find_last_outfile(cmd, outfile_count);
	if (!last_outfile)
		return (-1);
	if (last_outfile->type == OUTFILE)
		fd = open(last_outfile->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(last_outfile->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	return (fd);
}

int count_output(t_commandlist *cmd)
{
	t_file_node *current;
	int count;

	count = 0;
	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == OUTFILE || current->redir_type == OUTFILE_APPEND)
			count++;
		current = current->next;
	}
	return (count);
}
