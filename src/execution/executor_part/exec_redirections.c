/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirections.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:00:04 by kskender          #+#    #+#             */
/*   Updated: 2025/11/04 15:39:07 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int count_output(t_commandlist *cmd)
{
	t_filelist *current;
	int count;

	count = 0;
	current = cmd->files;
	while (current != NULL)
	{
		if (current->type == OUTFILE || current->type == OUTFILE_APPEND)
			count++;
		current = current->next;
	}
	return (count);
}

// INPUT STARTS HERE FOR REDIRECTIONS
static t_filelist *find_last_input(t_commandlist *cmd, int input_count)
{
	int current_count;
	t_filelist *current;

	current_count = 0;
	current = cmd->files;
	while (current != NULL)
	{
		if (current->type == INFILE || current->type == HEREDOC)
		{
			current_count++;
			if (current_count == input_count)
				return (current);
		}
		current = current->next;
	}
	return (NULL);
}

int count_input(t_commandlist *cmd)
{
	t_filelist *current;
	int count;

	count = 0;
	current = cmd->files;
	while (current != NULL)
	{
		if (current->type == INFILE || current->type == HEREDOC)
			count++;
		current = current->next;
	}
	return (count);
}

int setup_input_file(t_commandlist *cmd)
{
	int input_count;
	t_filelist *last_input;
	int fd;

	// t_gc *gc; // Removed unused variable
	input_count = count_input(cmd);
	if (input_count == 0)
		return (NO_REDIRECTION);
	last_input = find_last_input(cmd, input_count);
	if (last_input == NULL)
		return (NO_REDIRECTION);
	fd = gc_open(last_input->filename, O_RDONLY, 0);
	return (fd);
}

// Finds the last output redirection (for > and >>)
static t_filelist *find_last_output(t_commandlist *cmd, int output_count)
{
	int current_count;
	t_filelist *current;

	current_count = 0;
	current = cmd->files;
	while (current != NULL)
	{
		if (current->type == OUTFILE || current->type == OUTFILE_APPEND)
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
int setup_output_file(t_commandlist *cmd)
{
	int output_count;
	t_filelist *last_output;
	int fd;

	output_count = count_output(cmd);
	if (output_count == 0)
		return (NO_REDIRECTION);
	last_output = find_last_output(cmd, output_count);
	if (last_output == NULL)
		return (NO_REDIRECTION);
	if (last_output->type == OUTFILE)
		fd = gc_open(last_output->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = gc_open(last_output->filename, O_WRONLY | O_CREAT | O_APPEND,
					 0644);
	return (fd);
}

/* Wrapper helpers to allow passing parser-side command node */
int setup_input_file_from_cmd(t_cmd_node *cmd)
{
	t_commandlist tmp;

	tmp.files = (t_filelist *)cmd->files;
	return (setup_input_file(&tmp));
}

int setup_output_file_from_cmd(t_cmd_node *cmd)
{
	t_commandlist tmp;

	tmp.files = (t_filelist *)cmd->files;
	return (setup_output_file(&tmp));
}
