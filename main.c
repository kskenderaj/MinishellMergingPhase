/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 02:30:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 04:26:26 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "executor.h"
#include <unistd.h>

// Global shell state defined in globals.c
extern t_shell_state g_shell;
#ifdef DEBUG
void start_debug_segv_backtrace(void);
#endif

static void increment_shlvl(void)
{
	char *shlvl_str;
	int shlvl;

	shlvl_str = ft_getenv("SHLVL", g_shell.env);
	shlvl = 1;
	if (shlvl_str)
	{
		shlvl = ft_atoi(shlvl_str) + 1;
		free(shlvl_str);
	}
	shlvl_str = ft_itoa(shlvl);
	ft_setenv("SHLVL", shlvl_str, g_shell.env);
	free(shlvl_str);
}

int main(int argc, char **argv, char **envp)
{
	char *line;
	t_token_list *tokens;
	t_cmd_list *commands;
	int interactive;

	(void)argc;
	(void)argv;
#ifdef DEBUG
	start_debug_segv_backtrace();
#endif
	init_shell(envp); // Pass envp to initialize the environment
	start_signals();
	increment_shlvl();
	interactive = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
	while (1)
	{
		if (interactive)
			line = readline(PROMPT);
		else
		{
			static char buf[8192];
			if (!fgets(buf, sizeof(buf), stdin))
				line = NULL;
			else
			{
				size_t len = ft_strlen(buf);
				if (len && buf[len - 1] == '\n')
					buf[len - 1] = '\0';
				line = ft_strdup(buf);
			}
		}
		if (!line)
		{
			if (interactive)
				ft_putendl_fd("exit", STDOUT_FILENO);
			break;
		}
		if (interactive && *line)
			add_history(line);
		if (is_exit_command(line))
		{
			free(line);
			break;
		}
		tokens = tokenize_input(line);
		if (!tokens)
		{
			*exit_code() = 2;
			free(line);
			continue;
		}
		commands = parse_commands(tokens);
		if (!commands)
		{
			*exit_code() = 2;
			free_token_lst(&tokens);
			free(line);
			continue;
		}
		*exit_code() = execute_commands(commands);
		free_token_lst(&tokens);
		free_cmd_lst(&commands);
		free(line);
	}
#ifdef DEBUG
	write(STDERR_FILENO, "[DEBUG] leaving loop\n", 20);
#endif
#ifdef DEBUG
	write(STDERR_FILENO, "[DEBUG] calling cleanup_shell\n", 32);
#endif
	cleanup_shell();
#ifdef DEBUG
	write(STDERR_FILENO, "[DEBUG] after cleanup\n", 23);
#endif
	return (*exit_code());
}