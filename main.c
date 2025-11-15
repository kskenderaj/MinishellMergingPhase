/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 02:30:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 21:24:52 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "executor.h"
#include <unistd.h>

// Global shell state defined in globals.c
extern t_shell_state g_shell;

static void increment_shlvl(void)
{
	char *cur;
	int lvl;
	char *new_val;

	cur = ft_getenv("SHLVL", g_shell.env);
	lvl = 1;
	if (cur)
	{
		lvl = ft_atoi(cur) + 1;
		free(cur);
	}
	new_val = ft_itoa(lvl);
	if (!new_val)
		return;
	ft_setenv("SHLVL", new_val, g_shell.env);
	free(new_val);
}

static int is_interactive(void)
{
	return (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO));
}

static char *read_line_non_interactive(void)
{
	static char buf[8192];
	size_t len;

	if (!fgets(buf, sizeof(buf), stdin))
		return (NULL);
	len = ft_strlen(buf);
	if (len && buf[len - 1] == '\n')
		buf[len - 1] = '\0';
	return (ft_strdup(buf));
}

static char *read_next_line(int interactive)
{
	if (interactive)
		return (readline(PROMPT));
	return (read_line_non_interactive());
}

/* moved to driver.c */
int handle_null_line(char *line, int interactive);
void process_and_execute(char *line, int interactive);

int main(int argc, char **argv, char **envp)
{
	char *line;
	int interactive;

	(void)argc;
	(void)argv;
	init_shell(envp);
	start_signals();
	increment_shlvl();
	interactive = is_interactive();
	while (1)
	{
		line = read_next_line(interactive);
		if (handle_null_line(line, interactive) == -1)
			break;
		if (!line)
			continue;
		process_and_execute(line, interactive);
	}
	cleanup_shell();
	return (*exit_code());
}