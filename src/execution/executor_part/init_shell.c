/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 17:27:10 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 04:26:14 by klejdi           ###   ########.fr       */
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
#ifdef DEBUG
    t_env_node *cur = NULL;
    if (g_shell.env)
    {
        cur = g_shell.env->head;
        while (cur)
        {
            write(STDERR_FILENO, "[DEBUG] free env key=", 22);
            if (cur->key)
                write(STDERR_FILENO, cur->key, ft_strlen(cur->key));
            write(STDERR_FILENO, "\n", 1);
            cur = cur->next;
        }
    }
#endif
    if (g_shell.env)
    {
#ifdef DEBUG
        write(STDERR_FILENO, "[DEBUG] calling free_env_list()\n", 33);
#endif
        free_env_list(g_shell.env);
        g_shell.env = NULL;
#ifdef DEBUG
        write(STDERR_FILENO, "[DEBUG] finished free_env_list()\n", 34);
#endif
    }
    if (isatty(STDIN_FILENO))
    {
#ifdef DEBUG
        write(STDERR_FILENO, "[DEBUG] clearing history (interactive)\n", 43);
#endif
        clear_history();
    }
}