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
	t_file_node	*cur;

	count = 0;
	cur = (t_file_node *)cmd->files;
	while (cur)
	{
		if (cur->redir_type == HEREDOC)
			count++;
		cur = cur->next;
	}
	return (count);
}

// Finds the last heredoc
static t_file_node	*find_last_heredoc(t_commandlist *cmd, int heredoc_count)
{
	int			cur_count;
	t_file_node	*cur;

	cur_count = 0;
	cur = (t_file_node *)cmd->files;
	while (cur)
	{
		if (cur->redir_type == HEREDOC)
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
	t_file_node	*last_heredoc;
	int			tmpfd;
	char		template[] = "/tmp/minishell_heredoc_XXXXXX";
	size_t		len;

	heredoc_count = count_heredoc(cmd);
	if (heredoc_count == 0)
		return (-1);
	last_heredoc = find_last_heredoc(cmd, heredoc_count);
	if (!last_heredoc)
		return (-1);
	// Create temp file for heredoc content (even if empty)
	tmpfd = mkstemp(template);
	if (tmpfd == -1)
		return (-1);
	// Write the heredoc content to the temp file (if not empty)
	if (last_heredoc->heredoc_content)
	{
		len = ft_strlen(last_heredoc->heredoc_content);
		if (len > 0)
			write(tmpfd, last_heredoc->heredoc_content, len);
	}
	lseek(tmpfd, 0, SEEK_SET);
	return (tmpfd);
}
