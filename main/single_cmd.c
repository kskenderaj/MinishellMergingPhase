/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 20:17:09 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	is_empty_command(t_cmd_node *cmd)
{
	if (!cmd->cmd || !cmd->cmd[0])
		return (1);
	if (cmd->cmd[0] && !cmd->cmd[0][0])
		return (1);
	return (0);
}

static int	execute_builtin(t_cmd_node *cmd, char **envp)
{
	int	ret;

	if (cmd->env && cmd->env->size > 0)
		return (128);
	ret = table_of_builtins(cmd, envp, 1);
	return (ret);
}

static int	execute_external(t_cmd_node *cmd, char **envp, char **merged_envp)
{
	char	***per_cmd_env_array;
	int		ret;

	per_cmd_env_array = gc_malloc(sizeof(char **) * 1);
	per_cmd_env_array[0] = merged_envp;
	ret = exec_pipeline(&(cmd->cmd), 1, envp, &per_cmd_env_array);
	return (ret);
}

int	handle_single_command(t_cmd_node *cmd, t_env_list *env)
{
	t_cmd_exec_data	data;

	if (!cmd)
		return (0);
	data.envp = env_list_to_array(env);
	if (!data.envp)
		return (1);
	g_shell.current_envp = data.envp;
	data.merged_envp = merge_env_arrays(data.envp, cmd->env);
	if (!data.merged_envp)
		data.merged_envp = data.envp;
	if (setup_cmd_redirections(cmd, &data.fds) == -1)
	{
		cleanup_cmd_redir_failure(&data.fds, data.envp, data.merged_envp);
		return (1);
	}
	apply_cmd_redirections(&data.fds);
	if (is_empty_command(cmd))
	{
		handle_assignment_only(cmd);
		restore_cmd_fds(&data.fds);
		g_shell.current_envp = NULL;
		return (0);
	}
	data.ret = execute_builtin(cmd, data.envp);
	if (data.ret != 128)
	{
		restore_cmd_fds(&data.fds);
		g_shell.current_envp = NULL;
		return (data.ret);
	}
	data.ret = execute_external(cmd, data.envp, data.merged_envp);
	restore_cmd_fds(&data.fds);
	g_shell.current_envp = NULL;
	return (data.ret);
}
