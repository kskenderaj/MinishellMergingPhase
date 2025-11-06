/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_infile.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:25:42 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/03 13:47:36 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Counts infile/heredoc redirections
int	count_infile(t_commandlist *cmd)
{
	int			count;
	t_filelist	*cur;

	count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == INFILE || cur->type == HEREDOC)
			count++;
		cur = cur->next;
	}
	return (count);
}

// Finds the last infile/heredoc
static t_filelist	*find_last_infile(t_commandlist *cmd, int infile_count)
{
	int			cur_count;
	t_filelist	*cur;

	cur_count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == INFILE || cur->type == HEREDOC)
		{
			cur_count++;
			if (cur_count == infile_count)
				return (cur);
		}
		cur = cur->next;
	}
	return (NULL);
}

// Opens the last infile and returns fd
int	setup_infile_fd(t_commandlist *cmd)
{
	int			infile_count;
	t_filelist	*last_infile;
	int			fd;

	infile_count = count_infile(cmd);
	if (infile_count == 0)
		return (-1);
	last_infile = find_last_infile(cmd, infile_count);
	if (!last_infile)
		return (-1);
	fd = open(last_infile->filename, O_RDONLY);
	return (fd);
}
