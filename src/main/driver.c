#include "minishell.h"
#include "parser.h"
#include <readline/readline.h>
#include <readline/history.h>

int handle_null_line(char *line, int interactive)
{
    if (line)
        return (0);
    if (g_sigint_status)
    {
        *exit_code() = 1;
        g_sigint_status = 0;
        return (1);
    }
    if (interactive)
        ft_putendl_fd("exit", STDOUT_FILENO);
    return (-1);
}

void process_and_execute(char *line, int interactive)
{
    t_token_list *tokens;
    t_cmd_list *commands;

    if (interactive && *line)
        add_history(line);
    g_sigint_status = 0;
    if (is_exit_command(line))
    {
        free(line);
        return;
    }
    tokens = tokenize_input(line);
    if (!tokens)
    {
        *exit_code() = 2;
        free(line);
        return;
    }
    commands = parse_commands(tokens);
    if (!commands)
    {
        *exit_code() = 2;
        free_token_lst(&tokens);
        free(line);
        return;
    }
    *exit_code() = execute_commands(commands);
    free_token_lst(&tokens);
    free_cmd_lst(&commands);
    free(line);
}
