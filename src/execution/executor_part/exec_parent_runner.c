/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_parent_runner.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:14:57 by kskender          #+#    #+#             */
/*   Updated: 2025/11/04 15:39:10 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/*
1.sets input/output redirections for a comman and checks for errors
2. handles both input and output redirections with error checking
3. Returns EXIT_SUCCESS on success, or error code on failure
4. uses GC-tracked file descriptors to prevent leaks
*/

static int setting_infile_and_outfile(t_cmd_node *cmd, int *io_data)
{
	int error_code;

	error_code = 0;
	if (io_data[0] != NO_REDIRECTION)
	{
		error_code = setup_input_file_from_cmd(cmd);
		io_data[2] = NO_REDIRECTION;
		io_data[4] = NO_REDIRECTION;
		if (error_code != EXIT_SUCCESS)
			return (error_code);
	}
	if (io_data[1] != NO_REDIRECTION)
	{
		error_code = setup_output_file_from_cmd(cmd);
		io_data[2] = NO_REDIRECTION;
		io_data[4] = NO_REDIRECTION;
		if (error_code != EXIT_SUCCESS)
			return (error_code);
	}
	return (error_code);
}

pid_t exec_parent_runner(t_cmd_node *cmd, int *io_data)
{
	int error_code;

	error_code = setting_infile_and_outfile(cmd, io_data);
	dup_and_or_close(&io_data[2], &io_data[4]);
	if (error_code != EXIT_SUCCESS)
		return (*(cmd->env->pid));
	error_code = table_of_builtins(cmd, generate_env(cmd->env), 1);
	return (*(cmd->env->pid));
}
