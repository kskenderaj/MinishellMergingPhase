/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirections1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:43:35 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:25:39 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	count_output(t_commandlist *cmd)
{
	t_file_node	*current;
	int			count;

	count = 0;
	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == OUTFILE
			|| current->redir_type == OUTFILE_APPEND)
			count++;
		current = current->next;
	}
	return (count);
}

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
