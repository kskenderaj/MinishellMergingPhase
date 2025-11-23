/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers3.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:16:26 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:41:58 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void	apply_env_vars(char **envp, t_shell_state *shell)
{
	int		ei;
	char	*eq;
	char	*key;
	char	*val;

	if (!envp)
		return ;
	ei = 0;
	while (envp[ei])
	{
		eq = ft_strchr(envp[ei], '=');
		if (eq)
		{
			key = gc_substr(shell->gc, envp[ei], 0,
					(unsigned int)(eq - envp[ei]));
			val = eq + 1;
			if (key)
				setenv(key, val, 1);
		}
		ei++;
	}
}

void	execute_builtin(char **cmd, t_shell_state *shell)
{
	if (!strcmp(cmd[0], "echo"))
		ft_echo(cmd, shell);
	else if (!strcmp(cmd[0], "cd"))
		ft_cd(cmd, shell);
	else if (!strcmp(cmd[0], "pwd"))
		ft_pwd(cmd, shell);
	else if (!strcmp(cmd[0], "export"))
		ft_export(cmd, shell);
	else if (!strcmp(cmd[0], "unset"))
		ft_unset(cmd, shell);
	else if (!strcmp(cmd[0], "env"))
		ft_env(cmd, shell);
	else if (!strcmp(cmd[0], "exit"))
		ft_exit(cmd, shell);
	exit(0);
}

char	**get_cmd_env(t_pipeline_ctx *ctx, int i)
{
	if (ctx->per_cmd_envs && ctx->per_cmd_envs[i])
		return (ctx->per_cmd_envs[i]);
	return (ctx->envp);
}

void	close_fds(int in_fd, int out_fd, t_shell_state *shell)
{
	(void)shell;
	if (in_fd != -1 && in_fd != NO_REDIRECTION)
		close(in_fd);
	if (out_fd != -1 && out_fd != NO_REDIRECTION)
		close(out_fd);
}
