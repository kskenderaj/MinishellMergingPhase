#include "minishell.h"

// Placeholder for getting an environment variable's value.
char *ft_getenv(const char *name, t_env_list *env_list)
{
    t_env_node *current;

    current = env_list->head;
    while (current)
    {
        if (ft_strcmp(current->key, name) == 0)
            return (ft_strdup(current->value));
        current = current->next;
    }
    return (NULL);
}

// Placeholder for setting or updating an environment variable.
void ft_setenv(const char *name, const char *value, t_env_list *env_list)
{
    t_env_node *current;

    current = env_list->head;
    while (current)
    {
        if (ft_strcmp(current->key, name) == 0)
        {
            free(current->value);
            current->value = ft_strdup(value);
            return;
        }
        current = current->next;
    }
    // If not found, add it (implementation depends on your list structure)
    // This is a simplified add, your 'add_env' might be different.
    t_env_node *new_node = malloc(sizeof(t_env_node));
    if (new_node)
    {
        new_node->key = ft_strdup(name);
        new_node->value = ft_strdup(value);
        new_node->next = NULL;
        if (env_list->tail)
            env_list->tail->next = new_node;
        else
            env_list->head = new_node;
        env_list->tail = new_node;
    }
}

// Placeholder for getting an environment variable's value.
char *get_env_value(t_env_list *env, const char *key)
{
    return (ft_getenv(key, env));
}
