/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_cmd_env.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 19:56:31 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static t_env_node *find_existing_env(char *key)
{
    t_env_node *existing;

    existing = g_shell.env->head;
    while (existing)
    {
        if (ft_strcmp(existing->key, key) == 0)
            return (existing);
        existing = existing->next;
    }
    return (NULL);
}

static void update_existing_env(t_env_node *existing, char *value)
{
    free(existing->value);
    existing->value = ft_strdup(value);
}

static void add_new_env(char *key, char *value)
{
    t_env_node *new_node;

    new_node = malloc(sizeof(t_env_node));
    if (new_node)
    {
        new_node->key = ft_strdup(key);
        new_node->value = ft_strdup(value);
        push_env(g_shell.env, new_node);
    }
}

static void set_env_var(t_env_node *node)
{
    t_env_node *existing;

    setenv(node->key, node->value, 1);
    existing = find_existing_env(node->key);
    if (existing)
        update_existing_env(existing, node->value);
    else
        add_new_env(node->key, node->value);
}

int handle_assignment_only(t_cmd_node *cmd)
{
    t_env_node *node;

    if (!cmd->env || cmd->env->size == 0)
        return (0);
    node = cmd->env->head;
    while (node)
    {
        set_env_var(node);
        node = node->next;
    }
    g_shell.last_status = 0;
    return (1);
}
