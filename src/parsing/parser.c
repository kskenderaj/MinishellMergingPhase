/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 03:08:59 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 21:24:49 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

// Create a token instance
t_token *create_token(t_toktype type, char *val)
{
    t_token *token;

    token = (t_token *)malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    token->value = val;
    token->segment_list = NULL;
    token->next = NULL;
    return (token);
}

// Push a token to list tail
int push_token(t_token_list *lst, t_token *token)
{
    if (!lst || !token)
        return (1);
    if (!lst->head)
        lst->head = token;
    else
        lst->tail->next = token;
    lst->tail = token;
    lst->size++;
    return (0);
}

// Add token from raw slice (str,len) with given type
static char *dup_slice(char *str, int len)
{
    char *slice;
    slice = (char *)malloc(len + 1);
    if (!slice)
        return (NULL);
    ft_memcpy(slice, str, len);
    slice[len] = '\0';
    return (slice);
}

int add_token(t_token_list *lst, t_toktype type, char *str, int len)
{
    char *slice;
    t_token *token;
    if (!lst || !str || len <= 0)
        return (1);
    slice = dup_slice(str, len);
    if (!slice)
        return (1);
    token = create_token(type, slice);
    if (!token)
    {
        free(slice);
        return (1);
    }
    return (push_token(lst, token));
}

// Determine if a token type is any redirection
bool is_redirection(t_toktype t)
{
    return (t == T_REDIR_IN || t == T_REDIR_OUT || t == T_REDIR_APPEND || t == T_HEREDOC);
}

// A helper to add an argument to a command node's args array
static void add_arg_to_cmd(t_cmd_node *cmd, char *arg)
{
    int i;
    char **new_args;

    i = 0;
    if (cmd->cmd)
        while (cmd->cmd[i])
            i++;
    new_args = malloc(sizeof(char *) * (i + 2));
    if (!new_args)
        return;
    i = 0;
    if (cmd->cmd)
    {
        while (cmd->cmd[i])
        {
            new_args[i] = cmd->cmd[i];
            i++;
        }
    }
    new_args[i] = ft_strdup(arg);
    new_args[i + 1] = NULL;
    free(cmd->cmd);
    cmd->cmd = new_args;
}

// Main parsing function. Groups tokens into commands, separated by pipes.
// Main parsing function. Groups tokens into commands, separated by pipes.
t_cmd_list *parse_commands(t_token_list *tokens)
{
    t_cmd_list *cmd_list;
    t_cmd_node *current_cmd;
    t_token *tok;

    if (!tokens)
        return (NULL);
    cmd_list = (t_cmd_list *)malloc(sizeof(*cmd_list));
    if (!cmd_list)
        return (NULL);
    init_cmd_lst(cmd_list);
    current_cmd = create_cmdnode();
    if (!current_cmd)
    {
        free(cmd_list);
        return (NULL);
    }
    push_cmd(cmd_list, current_cmd);
    tok = tokens->head;
    while (tok)
    {
        if (tok->type == T_PIPE)
        {
            /* Start a new command after a pipe. Even if the previous
             * command had no args, keep the empty node to match pipe count. */
            current_cmd = create_cmdnode();
            if (!current_cmd)
            {
                // on allocation failure, free what we built so far
                // and signal parse error
                free_cmd_lst(&cmd_list);
                return (NULL);
            }
            push_cmd(cmd_list, current_cmd);
        }
        else
        {
            /* Keep redirection operators and their targets as-is in argv;
             * execution layer consumes them via setup_redirections(). */
            add_arg_to_cmd(current_cmd, tok->value);
        }
        tok = tok->next;
    }
    return (cmd_list);
}