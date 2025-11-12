/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_heredoc.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:28:18 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 02:41:24 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Counts heredoc redirections
int count_heredoc(t_commandlist *cmd)
{
	int count;
	t_filelist *cur;

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
static t_filelist *find_last_heredoc(t_commandlist *cmd, int heredoc_count)
{
	int cur_count;
	t_filelist *cur;

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
static int write_heredoc_to_temp(int tmpfd, t_filelist *last_heredoc)
{
	char *line;

	start_heredoc_signals();
	while (1)
	{
		line = readline("heredoc> ");
		if (g_sigint_status)
		{
			if (line)
				free(line);
			break;
		}
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", STDERR_FILENO);
			break;
		}
		if (ft_strcmp(line, last_heredoc->filename) == 0)
		{
			free(line);
			break;
		}
		ft_putendl_fd(line, tmpfd);
		free(line);
	}
	start_signals();
	lseek(tmpfd, 0, SEEK_SET);
	return (tmpfd);
}

int setup_heredoc_fd(t_commandlist *cmd)
{
	int heredoc_count;
	t_filelist *last_heredoc;
	int tmpfd;
	char template[] = "/tmp/minishell_heredoc_XXXXXX";

	heredoc_count = count_heredoc(cmd);
	if (heredoc_count == 0)
		return (-1);
	last_heredoc = find_last_heredoc(cmd, heredoc_count);
	if (!last_heredoc)
		return (-1);
	tmpfd = mkstemp(template);
	if (tmpfd == -1)
		return (-1);
	// Unlink the file immediately, so it's cleaned up on close
	unlink(template);
	return (write_heredoc_to_temp(tmpfd, last_heredoc));
}
