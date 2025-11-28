/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 13:49:58 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static char	*get_input_line(int interactive)
{
	if (interactive)
		return (readline(PROMPT));
	return (read_line_noninteractive());
}

static int	handle_single_line(char *line, t_env_list *env,
		t_shell_state *shell, int last_status)
{
	if (*line)
	{
		if (shell->is_interactive)
			add_history(line);
		within_command();
		last_status = process_input_line(line, env, last_status, shell);
		out_command();
	}
	return (last_status);
}

static int	process_line_cleanup(char *line, t_env_list *env,
		t_shell_state *shell, int last_status)
{
	last_status = handle_single_line(line, env, shell, last_status);
	free(line);
	shell->current_line = NULL;
	return (last_status);
}

int	main_loop(t_env_list *env, t_shell_state *shell)
{
	char	*line;
	int		last_status;
	int		interactive;

	last_status = 0;
	interactive = isatty(STDIN_FILENO);
	shell->is_interactive = interactive;
	while (1)
	{
		out_command();
		line = get_input_line(interactive);
		if (!line)
			break ;
		shell->current_line = line;
		last_status = process_line_cleanup(line, env, shell, last_status);
	}
	if (interactive)
	{
		write(STDOUT_FILENO, "exit\n", 5);
		rl_clear_history();
		rl_free_line_state();
		rl_cleanup_after_signal();
	}
	return (last_status);
}
