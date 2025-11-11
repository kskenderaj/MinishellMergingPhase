#include "minishell.h"

// Placeholder to check if a command is a builtin.
int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    if (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0 ||
        ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0 ||
        ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0 ||
        ft_strcmp(cmd, "exit") == 0)
        return (1);
    return (0);
}

// Placeholder for executing a builtin command, handling potential redirections.
int execute_builtin_with_possible_redir(t_cmd_node *cmd, t_env_list *env)
{
    (void)cmd;
    (void)env;
    return (0);
}
