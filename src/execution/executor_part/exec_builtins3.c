/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:06:11 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/04 16:49:37 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void print_exported_env(void)
{
	extern char **environ;
	int i;
	char *eq;
	int j;

	i = 0;
	while (environ[i])
	{
		eq = ft_strchr(environ[i], '=');
		if (eq)
		{
			write(STDOUT_FILENO, "declare -x ", 11);
			write(STDOUT_FILENO, environ[i], eq - environ[i]);
			write(STDOUT_FILENO, "=\"", 2);
			write(STDOUT_FILENO, eq + 1, ft_strlen(eq + 1));
			write(STDOUT_FILENO, "\"\n", 2);
		}
		else
		{
			write(STDOUT_FILENO, "declare -x ", 11);
			write(STDOUT_FILENO, environ[i], ft_strlen(environ[i]));
			write(STDOUT_FILENO, "\n", 1);
		}
		i++;
	}
	j = 0;
	while (j < g_shell.exported_count)
	{
		if (g_shell.exported_vars[j] && !getenv(g_shell.exported_vars[j]))
		{
			write(STDOUT_FILENO, "declare -x ", 11);
			write(STDOUT_FILENO, g_shell.exported_vars[j],
				  ft_strlen(g_shell.exported_vars[j]));
			write(STDOUT_FILENO, "\n", 1);
		}
		j++;
	}
}

int ft_env(char **args)
{
	extern char **environ;
	int i;

	i = 0;
	(void)args;
	while (environ[i])
	{
		ft_putstr_fd(environ[i], STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		i++;
	}
	return (0);
}

int ft_unset(char **args)
{
	int i;

	i = 1;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("unset: not a valid identifier: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putchar_fd('\n', STDERR_FILENO);
			i++;
			continue;
		}
		unsetenv(args[i]);
		i++;
	}
	return (0);
}
