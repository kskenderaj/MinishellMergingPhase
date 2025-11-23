/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:03:52 by kskender         ###   ########.fr       */
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

static int	execute_single_builtin(t_cmd_node *cmd, char **envp,
		t_shell_state *shell)
{
	int	ret;

	if (cmd->env && cmd->env->size > 0)
		return (128);
	ret = table_of_builtins(cmd, envp, 1, shell);
	return (ret);
}

static int	execute_external(t_cmd_node *cmd, char **envp, char **merged_envp,
		t_shell_state *shell)
{
	t_cmd_list	tmp_list;
	int			ret;

	(void)merged_envp;
	ft_memset(&tmp_list, 0, sizeof(t_cmd_list));
	tmp_list.head = cmd;
	tmp_list.size = 1;
	ret = exec_pipeline(&tmp_list, envp, shell);
	return (ret);
}

int	handle_single_command(t_cmd_node *cmd, t_env_list *env,
		t_shell_state *shell)
{
	t_cmd_exec_data	data;

	if (!cmd)
		return (0);
	data.envp = env_list_to_array(env, shell);
	if (!data.envp)
		return (1);
	shell->current_envp = data.envp;
	data.merged_envp = merge_env_arrays(data.envp, cmd->env);
	if (!data.merged_envp)
		data.merged_envp = data.envp;
	if (setup_cmd_redirections(cmd, &data.fds, shell) == -1)
	{
		cleanup_cmd_redir_failure(&data.fds, data.envp, data.merged_envp,
			shell);
		return (1);
	}
	apply_cmd_redirections(&data.fds, shell);
	if (is_empty_command(cmd))
	{
		handle_assignment_only(cmd, shell);
		restore_cmd_fds(&data.fds, shell);
		shell->current_envp = NULL;
		return (0);
	}
	data.ret = execute_single_builtin(cmd, data.envp, shell);
	if (data.ret != 128)
	{
		restore_cmd_fds(&data.fds, shell);
		shell->current_envp = NULL;
		return (data.ret);
	}
	data.ret = execute_external(cmd, data.envp, data.merged_envp, shell);
	restore_cmd_fds(&data.fds, shell);
	shell->current_envp = NULL;
	return (data.ret);
}
