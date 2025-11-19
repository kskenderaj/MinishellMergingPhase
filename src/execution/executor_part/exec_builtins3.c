/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:06:11 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 18:52:13 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

void	print_exported_env(void)
{
	extern char	**environ;
	int			i;
	char		*eq;
	int			j;

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

int	ft_env(char **args)
{
	extern char	**environ;
	int			i;

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

int	ft_unset(char **args)
{
	int	i;
	int	has_error;

	i = 1;
	has_error = 0;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("unset: not a valid identifier: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putchar_fd('\n', STDERR_FILENO);
			has_error = 1;
			i++;
			continue ;
		}
		unsetenv(args[i]);
		remove_from_env_list(g_shell.env, args[i]);
		i++;
	}
	g_shell.last_status = has_error;
	return (has_error);
}

static int	is_numeric(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static void	cleanup_and_exit(int exit_code)
{
	if (g_shell.current_line)
		free(g_shell.current_line);
	if (g_shell.current_envp)
		ft_free_array(g_shell.current_envp);
	gc_cleanup();
	if (g_shell.env)
		free_env_list(g_shell.env);
	exit(exit_code);
}

int	ft_exit(char **args)
{
	long	exit_code;
	int		i;
	int		is_printable;

	if (g_shell.is_interactive)
		ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!args[1])
		cleanup_and_exit(g_shell.last_status);
	/* Check if first argument is numeric BEFORE checking arg count */
	if (!is_numeric(args[1]))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		/* Print the argument - handle empty/null strings gracefully */
		if (args[1])
		{
			/* Check if string contains only printable chars */
			i = 0;
			is_printable = 1;
			while (args[1][i])
			{
				if (args[1][i] < 32 || args[1][i] > 126)
				{
					is_printable = 0;
					break ;
				}
				i++;
			}
			if (is_printable || i == 0)
				ft_putstr_fd(args[1], STDERR_FILENO);
		}
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		cleanup_and_exit(255);
	}
	/* Now check for too many arguments */
	if (args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		g_shell.last_status = 1;
		return (1);
	}
	exit_code = ft_atoi(args[1]);
	cleanup_and_exit((unsigned char)exit_code);
	return (0);
}
