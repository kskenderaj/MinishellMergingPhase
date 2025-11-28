/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:36:04 by kskender          #+#    #+#             */
/*   Updated: 2025/11/27 17:29:10 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

volatile sig_atomic_t	g_signal_status = 0;

void	sigint_exit(int sig)
{
	(void)sig;
	ft_putendl_fd("", 1);
	exit(130);
}

void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_status = 130;
	rl_done = 1;
}

void	within_command(void)
{
	g_signal_status = 1;
}

void	out_command(void)
{
	g_signal_status = 0;
}

void	parent_sigint_handler(int signo)
{
	(void)signo;
	if (g_signal_status == 0)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}
