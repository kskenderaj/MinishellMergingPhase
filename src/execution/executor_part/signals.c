/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:36:04 by kskender          #+#    #+#             */
/*   Updated: 2025/11/24 14:33:41 by jtoumani         ###   ########.fr       */
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

/* Global for signal handling - isolated from main data structures */
volatile sig_atomic_t	g_sigint_status = 0;

/* SIGINT handler for interactive prompt */
void	handle_sig_int(int signal_nb)
{
	(void)signal_nb;
	g_sigint_status = 130;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

void	start_signals(void)
{
	set_sigint_handler(handle_sig_int);
	set_sigquit_ignore();
	set_sigtstp_ignore();
}
