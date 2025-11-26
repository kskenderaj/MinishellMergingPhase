/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:16:23 by kskender          #+#    #+#             */
/*   Updated: 2025/11/26 16:04:45 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static void setup_child_fds(t_child_ctx *ctx)
{
	int j;

	if (ctx->in_fd != -1 && ctx->in_fd != NO_REDIRECTION)
		dup2(ctx->in_fd, STDIN_FILENO);
	else if (ctx->idx > 0)
		dup2(ctx->pipes[ctx->idx - 1][0], STDIN_FILENO);
	if (ctx->out_fd != -1 && ctx->out_fd != NO_REDIRECTION)
		dup2(ctx->out_fd, STDOUT_FILENO);
	else if (ctx->idx < ctx->ncmds - 1)
		dup2(ctx->pipes[ctx->idx][1], STDOUT_FILENO);
	j = 0;
	while (j < ctx->ncmds - 1)
	{
		close(ctx->pipes[j][0]);
		close(ctx->pipes[j][1]);
		j++;
	}
	if (ctx->in_fd != -1 && ctx->in_fd != NO_REDIRECTION)
		close(ctx->in_fd);
	if (ctx->out_fd != -1 && ctx->out_fd != NO_REDIRECTION)
		close(ctx->out_fd);
}

void setup_child_io_and_exec(t_child_ctx *ctx, t_shell_state *shell)
{
	setup_child_fds(ctx);
	apply_env_vars(ctx->envp, shell);
	if (!ctx->cmd || !ctx->cmd[0] || !ctx->cmd[0][0])
		exit(0);
	if (!strcmp(ctx->cmd[0], "echo") || !strcmp(ctx->cmd[0], "cd") || !strcmp(ctx->cmd[0], "pwd") || !strcmp(ctx->cmd[0], "export") || !strcmp(ctx->cmd[0], "unset") || !strcmp(ctx->cmd[0], "env") || !strcmp(ctx->cmd[0], "exit"))
		execute_builtin(ctx->cmd, shell);
	exec_external(ctx->cmd, ctx->envp, shell);
	exit(127);
}

static int validate_all_redirections(t_pipeline_ctx *ctx, t_shell_state *shell)
{
	t_cmd_node *node;
	int in_fd;
	int out_fd;
	int i;

	i = 0;
	node = ctx->cmdlst->head;
	while (i < ctx->ncmds && node)
	{
		in_fd = setup_input_file_from_cmd(node, shell);
		out_fd = setup_output_file_from_cmd(node, shell);
		if (in_fd >= 0)
			gc_close(shell->gc, in_fd);
		if (out_fd >= 0)
			gc_close(shell->gc, out_fd);
		if ((in_fd < 0 && in_fd != NO_REDIRECTION) || (out_fd < 0 && out_fd != NO_REDIRECTION))
		{
			shell->last_status = 1;
			return (1);
		}
		node = node->next;
		i++;
	}
	return (0);
}

static int setup_and_fork_child(t_child_init *init)
{
	int fds[2];

	fds[0] = setup_input_file_from_cmd(init->node, init->shell);
	fds[1] = setup_output_file_from_cmd(init->node, init->shell);
	init->in_fd = fds[0];
	init->out_fd = fds[1];
	init_child_ctx(init);
	init->ctx->pids[init->i] = fork();
	if (init->ctx->pids[init->i] == 0)
	{
		set_signals_child();
		setup_child_io_and_exec(init->child, init->shell);
	}
	close_fds(fds[0], fds[1], init->shell);
	return (0);
}

int spawn_pipeline_children(t_pipeline_ctx *ctx, t_shell_state *shell)
{
	int i;
	t_child_ctx child;
	t_cmd_node *node;
	t_child_init init;

	if (validate_all_redirections(ctx, shell) != 0)
		return (1);
	i = 0;
	node = ctx->cmdlst->head;
	init.child = &child;
	init.ctx = ctx;
	init.shell = shell;
	while (i < ctx->ncmds && node)
	{
		init.i = i;
		init.node = node;
		if (setup_and_fork_child(&init) != 0)
			return (1);
		node = node->next;
		i++;
	}
	return (0);
}
