/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_heredoc.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:28:18 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/06 14:52:04 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Counts heredoc redirections
int	count_heredoc(t_commandlist *cmd)
{
	int			count;
	t_filelist	*cur;

	count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == HEREDOC)
			count++;
		cur = cur->next;
	}
	return (count);
}

// Finds the last heredoc
static t_filelist	*find_last_heredoc(t_commandlist *cmd, int heredoc_count)
{
	int			cur_count;
	t_filelist	*cur;

	cur_count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == HEREDOC)
		{
			cur_count++;
			if (cur_count == heredoc_count)
				return (cur);
		}
		cur = cur->next;
	}
	return (NULL);
}

// Opens the last heredoc
int	setup_heredoc_fd(t_commandlist *cmd)
{
	int			heredoc_count;
	t_filelist	*last_heredoc;
	int			tmpfd;
	char		template[] = "/tmp/minishell_heredoc_XXXXXX";
	char		buffer[1024];
	size_t		len;
	int			d;

	heredoc_count = count_heredoc(cmd);
	if (heredoc_count == 0)
		return (-1);
	last_heredoc = find_last_heredoc(cmd, heredoc_count);
	if (!last_heredoc)
		return (-1);
	{
		/* ensure len is initialized before any debug use */
		len = strlen(last_heredoc->filename);
		tmpfd = mkstemp(template);
		if (tmpfd == -1)
			return (-1);
		/* Debug: write the heredoc delimiter to /tmp/heredoc_debug.txt */
		{
			d = open("tests/strict_tmp/heredoc_debug.txt",
					O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (d != -1)
			{
				printf("delimiter='%s' len=%zu\n", last_heredoc->filename, len);
				close(d);
			}
		}
		/* write heredoc content from stdin until delimiter */
		len = strlen(last_heredoc->filename);
		while (fgets(buffer, sizeof(buffer), stdin))
		{
			if (strncmp(buffer, last_heredoc->filename, len) == 0
				&& buffer[len] == '\n')
				break ;
			write(tmpfd, buffer, strlen(buffer));
		}
		/* reopen for reading at start */
		lseek(tmpfd, 0, SEEK_SET);
		return (tmpfd);
	}
}
