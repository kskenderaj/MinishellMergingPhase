/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirections.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:00:04 by kskender          #+#    #+#             */
/*   Updated: 2025/11/18 19:56:36 by klejdi           ###   ########.fr       */
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

// INPUT STARTS HERE FOR REDIRECTIONS
static t_file_node	*find_last_input(t_commandlist *cmd, int input_count)
{
	int			current_count;
	t_file_node	*current;

	current_count = 0;
	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == INFILE || current->redir_type == HEREDOC)
		{
			current_count++;
			if (current_count == input_count)
				return (current);
		}
		current = current->next;
	}
	return (NULL);
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

// Check all input files exist before opening (bash behavior)
// Returns 0 on success, -1 if any file doesn't exist
static int	check_all_input_files(t_commandlist *cmd)
{
	t_file_node	*current;
	int			fd;

	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == INFILE)
		{
			fd = open(current->filename, O_RDONLY);
			if (fd < 0)
			{
				perror(current->filename);
				g_shell.last_status = 1;
				return (-1);
			}
			close(fd);
		}
		current = current->next;
	}
	return (0);
}

int	setup_input_file(t_commandlist *cmd)
{
	int			input_count;
	t_file_node	*last_input;
	int			fd;

	input_count = count_input(cmd);
	if (input_count == 0)
		return (NO_REDIRECTION);
	if (check_all_input_files(cmd) == -1)
		return (-1);
	last_input = find_last_input(cmd, input_count);
	if (last_input == NULL)
		return (NO_REDIRECTION);
	if (last_input->redir_type == HEREDOC)
	{
		// Always use heredoc_content (read during parsing)
		// Even empty heredocs have content (empty string or NULL for truly empty)
		return (exec_heredoc_from_content(last_input->heredoc_content,
				last_input->heredoc_quoted));
	}
	fd = gc_open(last_input->filename, O_RDONLY, 0);
	if (fd < 0)
	{
		perror(last_input->filename);
		g_shell.last_status = 1;
	}
	return (fd);
}

// Opens all output files (bash behavior: all files are created)
// Returns 0 on success, -1 if any file fails to open
static int	open_all_output_files(t_commandlist *cmd)
{
	t_file_node	*current;
	int			fd;

	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == OUTFILE
			|| current->redir_type == OUTFILE_APPEND)
		{
			if (current->redir_type == OUTFILE)
				fd = gc_open(current->filename, O_WRONLY | O_CREAT | O_TRUNC,
						0644);
			else
				fd = gc_open(current->filename, O_WRONLY | O_CREAT | O_APPEND,
						0644);
			if (fd < 0)
			{
				perror(current->filename);
				g_shell.last_status = 1;
				return (-1);
			}
			close(fd);
		}
		current = current->next;
	}
	return (0);
}

// Finds the last output redirection (for > and >>)
static t_file_node	*find_last_output(t_commandlist *cmd, int output_count)
{
	int			current_count;
	t_file_node	*current;

	current_count = 0;
	current = (t_file_node *)cmd->files;
	while (current != NULL)
	{
		if (current->redir_type == OUTFILE
			|| current->redir_type == OUTFILE_APPEND)
		{
			current_count++;
			if (current_count == output_count)
				return (current);
		}
		current = current->next;
	}
	return (NULL);
}

// Opens the last output file with correct flags and returns fd
int	setup_output_file(t_commandlist *cmd)
{
	int			output_count;
	t_file_node	*last_output;
	int			fd;

	output_count = count_output(cmd);
	if (output_count == 0)
		return (NO_REDIRECTION);
	if (open_all_output_files(cmd) == -1)
		return (-1);
	last_output = find_last_output(cmd, output_count);
	if (last_output == NULL)
		return (NO_REDIRECTION);
	if (last_output->redir_type == OUTFILE)
		fd = gc_open(last_output->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = gc_open(last_output->filename, O_WRONLY | O_CREAT | O_APPEND,
				0644);
	if (fd < 0)
	{
		perror(last_output->filename);
		g_shell.last_status = 1;
	}
	return (fd);
}

/* Wrapper helpers to allow passing parser-side command node */
int	setup_input_file_from_cmd(t_cmd_node *cmd)
{
	t_commandlist	tmp;

	if (!cmd || !cmd->files || !cmd->files->head)
		return (NO_REDIRECTION);
	ft_memset(&tmp, 0, sizeof(t_commandlist));
	tmp.files = (t_filelist *)cmd->files->head;
	return (setup_input_file(&tmp));
}

int	setup_output_file_from_cmd(t_cmd_node *cmd)
{
	t_commandlist	tmp;

	if (!cmd || !cmd->files || !cmd->files->head)
		return (NO_REDIRECTION);
	ft_memset(&tmp, 0, sizeof(t_commandlist));
	tmp.files = (t_filelist *)cmd->files->head;
	return (setup_output_file(&tmp));
}
