/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 02:41:34 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 02:41:35 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void handle_sigint_interactive(int sig)
{
    (void)sig;
    *exit_code() = 1;
    ft_putchar_fd('\n', STDOUT_FILENO);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void handle_sigint_heredoc(int sig)
{
    (void)sig;
    // This is a global flag to stop the heredoc loop
    g_sigint_status = 1;
    *exit_code() = 1;
    // To exit readline, we simulate a newline
    ioctl(STDIN_FILENO, TIOCSTI, "\n");
}

void setup_signals_interactive(void)
{
    signal(SIGINT, handle_sigint_interactive);
    signal(SIGQUIT, SIG_IGN);
}

void setup_signals_heredoc(void)
{
    signal(SIGINT, handle_sigint_heredoc);
    signal(SIGQUIT, SIG_IGN);
}

void setup_signals_child(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
