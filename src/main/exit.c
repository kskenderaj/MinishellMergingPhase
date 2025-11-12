#include "minishell.h"

// Keep this as a simple prompt-quit check; full exit behavior is via builtin
int is_exit_command(char *line)
{
    if (!line)
        return (0);
    if (ft_strcmp(line, "exit") == 0)
        return (1);
    return (0);
}
