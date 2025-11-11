#include "minishell.h"

int run_external(char **args, t_env_list *env)
{
    (void)env;
    (void)args;
    return (127);
}

int build_and_run_pipeline(t_cmd_node *node, int n_cmds, t_env_list *env)
{
    (void)node;
    (void)n_cmds;
    (void)env;
    return (0);
}
