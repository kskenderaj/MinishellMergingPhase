/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_cmd_setup.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 17:42:58 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	save_std_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
	if (*saved_stdin < 0 || *saved_stdout < 0)
	{
		if (*saved_stdin >= 0)
			close(*saved_stdin);
		if (*saved_stdout >= 0)
			close(*saved_stdout);
		return (-1);
	}
	return (0);
}

int	setup_cmd_redirections(t_cmd_node *cmd, t_redir_fds *fds)
{
	int	saved_status;

	fds->saved_stdin = -1;
	fds->saved_stdout = -1;
	fds->in_fd = -1;
	fds->out_fd = -1;
	if (!cmd->files || !cmd->files->head)
		return (0);
	if (save_std_fds(&fds->saved_stdin, &fds->saved_stdout) == -1)
		return (-1);
	saved_status = g_shell.last_status;
	fds->out_fd = setup_output_file_from_cmd(cmd);
	if (g_shell.last_status == 1 && saved_status == 0)
		return (-1);
	fds->in_fd = setup_input_file_from_cmd(cmd);
	if (g_shell.last_status == 1 && saved_status == 0)
		return (-1);
	return (0);
}

void	apply_cmd_redirections(t_redir_fds *fds)
{
	if (fds->in_fd >= 0 && fds->in_fd != NO_REDIRECTION)
	{
		dup2(fds->in_fd, STDIN_FILENO);
		close(fds->in_fd);
	}
	if (fds->out_fd >= 0 && fds->out_fd != NO_REDIRECTION)
	{
		dup2(fds->out_fd, STDOUT_FILENO);
		close(fds->out_fd);
	}
}

void	restore_cmd_fds(t_redir_fds *fds)
{
	if (fds->saved_stdin >= 0)
	{
		dup2(fds->saved_stdin, STDIN_FILENO);
		close(fds->saved_stdin);
	}
	if (fds->saved_stdout >= 0)
	{
		dup2(fds->saved_stdout, STDOUT_FILENO);
		close(fds->saved_stdout);
	}
}

void	cleanup_cmd_redir_failure(t_redir_fds *fds, char **envp, char **merged)
{
	restore_cmd_fds(fds);
	if (fds->in_fd >= 0)
		close(fds->in_fd);
	if (fds->out_fd >= 0)
		close(fds->out_fd);
	if (merged != envp)
		ft_free_array(merged);
	ft_free_array(envp);
}
