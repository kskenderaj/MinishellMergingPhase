#include "minishell.h"
#include "garbage_collector.h"

// Placeholder for freeing the token list.
void free_token_lst(t_token_list **lst)
{
    t_token *current;
    t_token *next;

    if (!lst || !*lst)
        return;
    current = (*lst)->head;
    while (current)
    {
        next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
    free(*lst);
    *lst = NULL;
}

// Placeholder for freeing the command list.
void free_cmd_lst(t_cmd_list **lst)
{
    t_cmd_node *current;
    t_cmd_node *next;
    t_file_node *fcur;
    t_file_node *fnext;

    if (!lst || !*lst)
        return;
    current = (*lst)->head;
    while (current)
    {
        next = current->next;
        // Assuming ft_free_array handles the char**
        ft_free_array(current->cmd);
        if (current->files)
        {
            fcur = current->files->head;
            while (fcur)
            {
                fnext = fcur->next;
                free(fcur->filename);
                free(fcur);
                fcur = fnext;
            }
            free(current->files);
        }
        free(current);
        current = next;
    }
    free(*lst);
    *lst = NULL;
}

/* free_env_list provided elsewhere (avoid duplicate symbol) */
