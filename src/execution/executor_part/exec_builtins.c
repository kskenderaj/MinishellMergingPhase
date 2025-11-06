/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:32:23 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/04 15:22:03 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int table_of_builtins(t_cmd_node *cmd, char **envp, int flag)
{
	if (!cmd || !cmd->cmd || !cmd->cmd[0])
		return (-1);
	if (!strcmp(cmd->cmd[0], "echo"))
		return (ft_echo(cmd->cmd));
	if (!strcmp(cmd->cmd[0], "pwd"))
		return (ft_pwd(cmd->cmd));
	if (!strcmp(cmd->cmd[0], "cd"))
		return (ft_cd(cmd->cmd));
	if (!strcmp(cmd->cmd[0], "export"))
		return (ft_export(cmd->cmd));
	if (!strcmp(cmd->cmd[0], "unset"))
		return (ft_unset(cmd->cmd));
	if (!strcmp(cmd->cmd[0], "env"))
		return (ft_env(cmd->cmd));
	/* not a builtin */
	(void)envp;
	(void)flag;
	return (128);
}

int ft_pwd(char **args)
{
	char cwd[4096];

	(void)args;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

/*
** Quick compatibility shim for builtins: construct envp from the process
** environ so that calls to setenv()/unsetenv() are immediately visible.
*/
char **generate_env(t_env_list *env)
{
	extern char **environ;
	char **envp;
	int i;

	(void)env;
	if (!environ)
		return (NULL);
	i = 0;
	while (environ[i])
		i++;
	envp = gc_malloc(sizeof(char *) * (i + 1));
	if (!envp)
		return (NULL);
	i = 0;
	while (environ[i])
	{
		envp[i] = ft_strdup(environ[i]);
		i++;
	}
	envp[i] = NULL;
	return (envp);
}