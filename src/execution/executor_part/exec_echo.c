/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:22:40 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 14:34:19 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static int is_n_option(char *arg)
{
	int i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

static void echo_expand_and_print(char *arg)
{
	if (arg)
		ft_putstr_fd(arg, STDOUT_FILENO);
}

static void echo_print_range(char **args, int start_index)
{
	int i;

	i = start_index;
	while (args[i])
	{
		echo_expand_and_print(args[i]);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
}

int ft_echo(char **args, t_shell_state *shell)
{
	int nl_option;
	int start_index;

	(void)shell;
	nl_option = 0;
	start_index = 1;
	if (args[1])
	{
		while (args[start_index] && is_n_option(args[start_index]))
		{
			nl_option = 1;
			start_index++;
		}
	}
	echo_print_range(args, start_index);
	if (!nl_option)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
