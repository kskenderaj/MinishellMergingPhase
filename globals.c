/* globals.c — single definition of g_shell */

#include "executor.h"

/* Ensure default empty exported lists */

t_shell_state g_shell = {{NULL}, 0, 0, NULL};

t_env_list *setup_env_list(void)
{
    t_env_list *env_list;
    extern char **environ;
    char **env;
    t_env_node *node;

    env_list = gc_malloc(sizeof(t_env_list));
    if (!env_list)
        return (NULL);
    init_env_lst(env_list);
    env = environ;
    while (*env)
    {
        char *eq = ft_strchr(*env, '=');
        if (eq)
        {
            char *key = gc_substr(*env, 0, (unsigned int)(eq - *env));
            char *value = gc_strdup(eq + 1);
            node = gc_malloc(sizeof(t_env_node));
            if (node)
            {
                node->key = key;
                node->value = value;
                node->next = NULL;
                if (!env_list->head)
                {
                    env_list->head = node;
                    env_list->tail = node;
                }
                else
                {
                    env_list->tail->next = node;
                    env_list->tail = node;
                }
                env_list->size++;
            }
        }
        env++;
    }
    return (env_list);
}
