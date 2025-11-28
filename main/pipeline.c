/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/28 13:29:38 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	handle_pipeline(t_cmd_list *cmdlst, t_env_list *env, t_shell_state *shell)
{
	char	**envp;
	int		ret;

	envp = env_list_to_array(env, shell);
	if (!envp)
		return (1);
	ret = exec_pipeline(cmdlst, envp, shell);
	return (ret);
}

int	is_empty_command(t_cmd_node *cmd)
{
	if (!cmd->cmd || !cmd->cmd[0])
		return (1);
	if (cmd->cmd[0] && !cmd->cmd[0][0])
		return (1);
	return (0);
}
