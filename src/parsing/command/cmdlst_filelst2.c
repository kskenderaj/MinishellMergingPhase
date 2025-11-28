/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdlst_filelst2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/28 13:26:29 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_file_node	*read_all_heredocs_in_cmd(t_cmd_node *cmd, t_shell_state *shell)
{
	t_file_node	*current;
	t_file_node	*last_heredoc;

	if (!cmd || !cmd->files)
		return (NULL);
	last_heredoc = NULL;
	current = cmd->files->head;
	while (current)
	{
		if (current->redir_type == 6)
		{
			if (isatty(STDIN_FILENO) && g_signal_status != 130)
			{
				current->heredoc_content = read_heredoc_content
					(current->filename, shell);
				if (g_signal_status == 130)
					break ;
				if (last_heredoc)
					last_heredoc->heredoc_content = NULL;
			}
			last_heredoc = current;
		}
		current = current->next;
	}
	return (last_heredoc);
}

void	process_all_heredocs(t_cmd_list *cmdlst, t_shell_state *shell)
{
	t_cmd_node	*cmd;

	if (!cmdlst)
		return ;
	cmd = cmdlst->head;
	while (cmd)
	{
		read_all_heredocs_in_cmd(cmd, shell);
		if (g_signal_status == 130)
			return ;
		cmd = cmd->next;
	}
}
