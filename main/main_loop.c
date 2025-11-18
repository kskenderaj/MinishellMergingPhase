/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 19:56:42 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int handle_eof(int interactive)
{
    if (interactive && g_sigint_status == 130)
    {
        g_sigint_status = 0;
        return (1);
    }
    return (0);
}

static void handle_sigint(int *last_status)
{
    if (g_sigint_status == 130)
    {
        *last_status = 130;
        g_sigint_status = 0;
    }
}

int main_loop(t_env_list *env)
{
    char *line;
    int last_status;
    int interactive;

    last_status = 0;
    interactive = isatty(STDIN_FILENO);
    g_shell.is_interactive = interactive;
    while (1)
    {
        if (interactive)
            line = readline(PROMPT);
        else
            line = read_line_noninteractive();
        if (!line)
        {
            if (handle_eof(interactive))
                continue;
            break;
        }
        handle_sigint(&last_status);
        if (*line)
        {
            if (interactive)
                add_history(line);
            last_status = process_input_line(line, env, last_status);
        }
        free(line);
        gc_clear();
    }
    if (interactive)
        write(STDOUT_FILENO, "exit\n", 5);
    return (last_status);
}
