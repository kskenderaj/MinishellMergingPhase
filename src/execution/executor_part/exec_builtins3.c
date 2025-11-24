/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:06:11 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/24 13:57:12 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

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

static void	cleanup_and_exit(int exit_code, t_shell_state *shell)
{
	if (shell->current_line)
		free(shell->current_line);
	gc_cleanup(shell->gc);
	if (shell->env)
		free_env_list(shell->env);
	exit(exit_code);
}

static void	exit_print_error(char *arg)
{
	int	i;
	int	is_printable;

	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	if (arg)
	{
		i = 0;
		is_printable = 1;
		while (arg[i])
		{
			if (arg[i] < 32 || arg[i] > 126)
			{
				is_printable = 0;
				break ;
			}
			i++;
		}
		if (is_printable || i == 0)
			ft_putstr_fd(arg, STDERR_FILENO);
	}
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
}

int	ft_exit(char **args, t_shell_state *shell)
{
	long	exit_code;

	if (shell->is_interactive)
		ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (!args[1])
		cleanup_and_exit(shell->last_status, shell);
	if (!is_numeric(args[1]))
	{
		exit_print_error(args[1]);
		cleanup_and_exit(255, shell);
	}
	if (args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		shell->last_status = 1;
		return (1);
	}
	exit_code = ft_atoi(args[1]) % 256;
	cleanup_and_exit(exit_code, shell);
	return (0);
}
