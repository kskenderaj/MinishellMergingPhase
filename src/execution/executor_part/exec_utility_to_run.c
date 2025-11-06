/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utility_to_run.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 13:12:19 by kskender          #+#    #+#             */
/*   Updated: 2025/11/03 14:17:02 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <stdio.h>

// Stub for not_error_file
int	not_error_file(t_filelist *current, int update, t_commandlist *cmd)
{
	(void)cmd;
	if (current && current->filename)
	{
		ft_putstr_fd(current->filename, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
	}
	else
		ft_putstr_fd("input file error\n", STDERR_FILENO);
	(void)update;
	return (-1);
}

// Stub for standard_error
int	standard_error(int update, t_commandlist *cmd)
{
	(void)cmd;
	if (update)
		ft_putstr_fd("redirection/setup error\n", STDERR_FILENO);
	else
		ft_putstr_fd("error\n", STDERR_FILENO);
	return (-1);
}
/*
1.opens and redirects a regualr input file to standard input
2.handling errors if file doesnt exist
*/

int	handling_the_infile(t_commandlist *cmd, t_filelist *current, int update)
{
	int	fd;

	fd = gc_open(current->filename, O_RDONLY);
	if (fd == -1)
		return (not_error_file(current, update, cmd));
	if (gc_dup2(fd, STDIN_FILENO) == -1)
		return (gc_close(fd), standard_error(update, cmd));
	return (gc_close(fd), EXIT_SUCCESS);
}

/*
1.sets up the final input redirection by processing all input files
2. and the heredocs then applying the last one to standard input
*/
int	handling_the_heredoc(t_commandlist *cmd, t_filelist *current, int update)
{
	int	heredoc_pipe[2];

	gc_pipe(heredoc_pipe);
	if (current->filename != NULL && (*current->filename))
		ft_putstr_fd(current->filename, heredoc_pipe[1]);
	ft_putstr_fd("\n", heredoc_pipe[1]);
	gc_close(heredoc_pipe[1]);
	if (gc_dup2(heredoc_pipe[0], STDIN_FILENO) == -1)
		return (gc_close(heredoc_pipe[0]), standard_error(update, cmd));
	gc_close(heredoc_pipe[0]);
	return (EXIT_SUCCESS);
}

/*
1.sets up the final input redirection by processing all input files
2.and the heredocs then applying the last one to standard input
*/
int	setup_input(t_commandlist *cmd, int *redirect, int update)
{
	(void)cmd;
	(void)redirect;
	(void)update;
	return (0);
}
