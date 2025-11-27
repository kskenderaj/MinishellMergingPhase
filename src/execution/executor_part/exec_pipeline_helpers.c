/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 16:04:59 by kskender          #+#    #+#             */
/*   Updated: 2025/11/27 17:53:04 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "main.h"

static int **allocate_pipes(int ncmds, t_shell_state *shell)
{
	int **pipes;
	int i;

	pipes = gc_malloc(shell->gc, sizeof(int *) * (ncmds - 1));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < ncmds - 1)
	{
		pipes[i] = gc_malloc(shell->gc, sizeof(int) * 2);
		if (!pipes[i])
			return (NULL);
		i++;
	}
	return (pipes);
}

static int init_pipeline_ctx(t_pipeline_ctx *ctx, t_cmd_list *cmdlst,
							 char **envp, t_shell_state *shell)
{
	ctx->pipes = allocate_pipes((int)cmdlst->size, shell);
	if (!ctx->pipes)
		return (1);
	ctx->pids = gc_malloc(shell->gc, sizeof(pid_t) * cmdlst->size);
	if (!ctx->pids)
		return (1);
	ctx->cmdlst = cmdlst;
	ctx->ncmds = (int)cmdlst->size;
	ctx->envp = envp;
	ctx->per_cmd_envs = gc_malloc(shell->gc, sizeof(char **) * cmdlst->size);
	if (!ctx->per_cmd_envs)
		return (1);
	return (0);
}

static void setup_per_cmd_envs(t_pipeline_ctx *ctx, char **envp)
{
	t_cmd_node *node;
	int i;

	i = 0;
	node = ctx->cmdlst->head;
	while (node && i < ctx->ncmds)
	{
		if (node->env && node->env->size > 0)
			ctx->per_cmd_envs[i] = merge_env_arrays(envp, node->env);
		else
			ctx->per_cmd_envs[i] = NULL;
		node = node->next;
		i++;
	}
}

int exec_pipeline(t_cmd_list *cmdlst, char **envp, t_shell_state *shell)
{
	t_pipeline_ctx ctx;
	int ret;

	if (!cmdlst || cmdlst->size <= 0)
		return (1);
	if (init_pipeline_ctx(&ctx, cmdlst, envp, shell) != 0)
		return (1);
	setup_per_cmd_envs(&ctx, envp);
	if (create_pipes(ctx.pipes, ctx.ncmds, shell) != 0)
		return (1);
	if (spawn_pipeline_children(&ctx, shell) != 0)
	{
		close_all_pipes(ctx.pipes, ctx.ncmds, shell);
		return (1);
	}
	close_all_pipes(ctx.pipes, ctx.ncmds, shell);
	ret = wait_children(ctx.pids, ctx.ncmds, shell);
	set_signals_parent();
	return (ret);
}
