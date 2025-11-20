/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:36:04 by kskender          #+#    #+#             */
/*   Updated: 2025/11/19 16:36:05 by kskender         ###   ########.fr       */
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

/* Global observed elsewhere when a SIGINT occurred */
volatile sig_atomic_t	g_sigint_status = 0;

/* Disable terminal echoing of control characters (e.g. '^C') */
void	remove_ctrlc_echo(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return ;
#ifdef ECHOCTL
	term.c_lflag &= ~ECHOCTL;
	(void)tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}

/* SIGINT handler for interactive prompt */
void	handle_sig_int(int signal_nb)
{
	(void)signal_nb;
	g_sigint_status = 130; // Set exit status to 130 (128 + SIGINT)
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
#ifdef RL_REPLACE_LINE
	rl_replace_line("", 0);
#endif
}

/* SIGINT handler used while reading a heredoc */
void	handle_ctrlc_heredoc(int signal_nb)
{
	(void)signal_nb;
	g_sigint_status = 130; // Set exit status to 130 (128 + SIGINT)
	write(STDOUT_FILENO, "\n", 1);
}

static void	set_sigint_handler(void (*handler)(int))
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	/* Don't use SA_RESTART so readline() returns NULL on SIGINT */
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}

static void	set_sigquit_ignore(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGQUIT, &sa, NULL);
}

/* Install handlers for interactive prompt */
void	start_signals(void)
{
	set_sigint_handler(handle_sig_int);
	set_sigquit_ignore();
	remove_ctrlc_echo();
}

/* Install handlers appropriate for heredoc reading */
void	start_heredoc_signals(void)
{
	set_sigint_handler(handle_ctrlc_heredoc);
	set_sigquit_ignore();
}

/* Reset signal handlers to default for child processes */
void	reset_signals_for_child(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
