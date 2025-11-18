/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_merge.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 19:56:43 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static char **allocate_merged_array(char **base_envp, t_env_list *cmd_env)
{
    char **merged;
    int i;

    i = 0;
    while (base_envp[i])
        i++;
    merged = malloc(sizeof(char *) * (i + cmd_env->size + 1));
    return (merged);
}

static int copy_base_env(char **merged, char **base_envp)
{
    int i;

    i = 0;
    while (base_envp[i])
    {
        merged[i] = ft_strdup(base_envp[i]);
        if (!merged[i])
        {
            while (--i >= 0)
                free(merged[i]);
            free(merged);
            return (-1);
        }
        i++;
    }
    return (i);
}

static int add_cmd_env(char **merged, int j, t_env_list *cmd_env)
{
    t_env_node *node;
    char *temp;

    node = cmd_env->head;
    while (node)
    {
        temp = ft_strjoin(node->key, "=");
        if (temp)
        {
            merged[j] = ft_strjoin(temp, node->value);
            free(temp);
            if (merged[j])
                j++;
        }
        node = node->next;
    }
    return (j);
}

char **merge_env_arrays(char **base_envp, t_env_list *cmd_env)
{
    char **merged;
    int i;
    int j;

    if (!cmd_env || cmd_env->size == 0)
        return (base_envp);
    merged = allocate_merged_array(base_envp, cmd_env);
    if (!merged)
        return (base_envp);
    i = copy_base_env(merged, base_envp);
    if (i == -1)
        return (base_envp);
    j = add_cmd_env(merged, i, cmd_env);
    merged[j] = NULL;
    return (merged);
}
