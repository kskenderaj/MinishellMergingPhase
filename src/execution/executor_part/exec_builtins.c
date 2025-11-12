/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:32:23 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 02:56:47 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "builtins.h"

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
	if (!strcmp(cmd->cmd[0], "exit"))
		return (ft_exit(cmd->cmd));
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

char **generate_env(t_env_list *env)
{
	int count;
	char **envp;
	t_env_node *cur;
	int i;
	char *line;
	int len;

	if (!env)
		return (NULL);
	count = 0;
	cur = env->head;
	while (cur)
	{
		count++;
		cur = cur->next;
	}
	envp = gc_malloc(sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	cur = env->head;
	i = 0;
	while (cur)
	{
		if (cur->key)
		{
			len = ft_strlen(cur->key) + 1;
			if (cur->value)
				len += ft_strlen(cur->value);
			line = gc_malloc(len + 1);
			if (!line)
				return (envp);
			line[0] = '\0';
			ft_strlcpy(line, cur->key, len + 1);
			ft_strlcat(line, "=", len + 1);
			if (cur->value)
				ft_strlcat(line, cur->value, len + 1);
			envp[i++] = line;
		}
		cur = cur->next;
	}
	envp[i] = NULL;
	return (envp);
}

static int is_numeric(const char *s)
{
	int i;

	if (!s || !*s)
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

static int parse_exit_status(const char *s, int *out)
{
	long long val;
	int sign;
	int i;

	if (!s || !out)
		return (0);
	sign = 1;
	i = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	val = 0;
	while (s[i])
	{
		if (val > 922337203685477580LL)
			return (0);
		val = val * 10 + (s[i] - '0');
		if (val < 0)
			return (0);
		i++;
	}
	val *= sign;
	*out = (int)((unsigned char)(val & 0xFF));
	return (1);
}

int ft_exit(char **args)
{
	int argc;
	int code;

	argc = 0;
	while (args[argc])
		argc++;
	if (argc == 1)
	{
		cleanup_shell();
		exit((unsigned char)g_shell.last_status);
	}
	if (!is_numeric(args[1]))
	{
		ft_putstr_fd("exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		cleanup_shell();
		exit(255);
	}
	if (argc > 2)
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		g_shell.last_status = 1;
		return (1);
	}
	if (!parse_exit_status(args[1], &code))
	{
		ft_putstr_fd("exit: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		cleanup_shell();
		exit(255);
	}
	cleanup_shell();
	exit(code);
}