/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers1.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:16:20 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:41:53 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void	close_all_pipes(int **pipes, int ncmds, t_shell_state *shell)
{
	int	i;

	(void)shell;
	i = 0;
	while (i < ncmds - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

int	wait_children(pid_t *pids, int ncmds, t_shell_state *shell)
{
	int	i;
	int	status;

	(void)shell;
	status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	i = 0;
	while (i < ncmds)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	start_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (128);
}

int	create_pipes(int **pipes, int ncmds, t_shell_state *shell)
{
	int	i;

	(void)shell;
	i = 0;
	while (i < ncmds - 1)
	{
		if (pipe(pipes[i]) == -1)
			return (1);
		i++;
	}
	return (0);
}

void	init_child_ctx(t_child_init *init)
{
	init->child->idx = init->i;
	init->child->ncmds = init->ctx->ncmds;
	init->child->pipes = init->ctx->pipes;
	init->child->in_fd = init->in_fd;
	init->child->out_fd = init->out_fd;
	init->child->cmd_node = init->node;
	init->child->cmd = init->node->cmd;
	init->child->envp = get_cmd_env(init->ctx, init->i);
}
