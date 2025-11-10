#include "main_helpers.h"
#include "executor.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <ctype.h>

char *get_input_line(void)
{
    char *input = NULL;
    if (isatty(STDIN_FILENO))
    {
        input = readline("minishell$ ");
        if (input && *input)
            add_history(input);
    }
    else
    {
        char buf[1024];
        if (!fgets(buf, sizeof(buf), stdin))
            return (NULL);
        buf[strcspn(buf, "\n\r")] = 0;
        input = strdup(buf);
    }
    return (input);
}

int is_comment_line(const char *s)
{
    const char *p = s;
    while (*p && isspace((unsigned char)*p))
        p++;
    return (*p == '#');
}

int is_exit_line(const char *s)
{
    const char *p = s;
    long code;
    char *end;

    while (*p && isspace((unsigned char)*p))
        p++;
    if (strncmp(p, "exit", 4) != 0)
        return (0);
    p += 4;
    while (*p && isspace((unsigned char)*p))
        p++;
    if (!*p)
    {
        g_shell.last_status = 0;
        return (1);
    }
    const char *start = p;
    code = strtol(p, &end, 10);
    if (end == start)
    {
        // not a number
        g_shell.last_status = 2;
        return (1);
    }
    while (*end && isspace((unsigned char)*end))
        end++;
    if (*end)
    {
        // extra args
        g_shell.last_status = 1; // bash does exit: too many arguments, exit 1
        return (1);
    }
    // clamp to 0-255
    g_shell.last_status = (int)((unsigned char)code);
    return (1);
}
