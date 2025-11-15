/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 17:27:10 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/14 23:02:30 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "parser.h"

void init_shell(char **envp)
{
    int i;

    g_shell.exported_count = 0;
    i = 0;
    while (i < MAX_EXPORTED)
    {
        g_shell.exported_vars[i] = NULL;
        i++;
    }
    g_shell.env = setup_env_list(envp);
}

void cleanup_shell(void)
{
    if (g_shell.env)
    {
        free_env_list(g_shell.env);
        g_shell.env = NULL;
    }
    if (isatty(STDIN_FILENO))
        clear_history();
    gc_cleanup();
}