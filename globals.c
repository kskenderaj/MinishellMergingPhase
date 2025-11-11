/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 01:53:44 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 01:54:34 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* Ensure default empty exported lists */

t_shell_state g_shell = {{NULL}, 0, 0, NULL};

t_env_list *setup_env_list(char **envp)
{
    t_env_list *env_list;
    char *key;
    char *value;
    int i;
    char *eq_ptr;

    env_list = malloc(sizeof(t_env_list));
    if (!env_list)
        return (NULL);
    init_env_lst(env_list);
    i = 0;
    while (envp && envp[i])
    {
        eq_ptr = ft_strchr(envp[i], '=');
        if (eq_ptr)
        {
            key = ft_substr(envp[i], 0, eq_ptr - envp[i]);
            value = ft_strdup(eq_ptr + 1);
            ft_setenv(key, value, env_list);
            free(key);
            free(value);
        }
        i++;
    }
    return (env_list);
}
