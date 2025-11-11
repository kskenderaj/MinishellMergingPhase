#include "minishell.h"

// Placeholder to check if the command is 'exit'.
int is_exit_command(char *line)
{
    if (line && ft_strcmp(line, "exit") == 0)
        return (1);
    return (0);
}
