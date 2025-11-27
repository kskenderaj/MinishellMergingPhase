/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_processing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/27 17:55:56 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int process_command(t_cmd_list *cmdlst, t_env_list *envlst,
					t_shell_state *shell)
{
	int ret;

	if (!cmdlst || !cmdlst->head)
		return (0);
	if (cmdlst->size == 1)
		ret = handle_single_command(cmdlst->head, envlst, shell);
	else
		ret = handle_pipeline(cmdlst, envlst, shell);
	shell->last_status = ret;
	return (ret);
}

static int handle_syntax_error(t_cmd_list *cmdlst, t_shell_state *shell)
{
	if (cmdlst->syntax_error)
		ft_putendl_fd("minishell: syntax error", 2);
	gc_clear(shell->gc);
	return (2);
}

int process_input_line(char *line, t_env_list *env, int last_status,
					   t_shell_state *shell)
{
	t_token_list toklst;
	t_cmd_list cmdlst;

	if (!line || !*line)
		return (last_status);
	init_token_lst(&toklst);
	init_cmd_lst(&cmdlst);
	if (tokenize(&toklst, line, shell) != 0)
		return (handle_syntax_error(&cmdlst, shell));
	if (token_to_cmd(&toklst, &cmdlst, shell) != 0)
		return (handle_syntax_error(&cmdlst, shell));
	if (cmdlst.syntax_error)
		return (handle_syntax_error(&cmdlst, shell));
	process_all_heredocs(&cmdlst, shell);
	if(g_signal_status == 130)
	{
		gc_clear(shell->gc);
		shell->last_status = 130;
		g_signal_status = 0;
		return (130);
	}
	if (cmdlst.head)
	{
		last_status = process_command(&cmdlst, env, shell);
		gc_clear(shell->gc);
		return (last_status);
	}
	gc_clear(shell->gc);
	return (0);
}

static int append_char(char **line, char c, int *len, int *capacity)
{
	char *tmp;

	if (*len >= *capacity)
	{
		if (*capacity == 0)
			*capacity = 64;
		else
			*capacity *= 2;
		tmp = ft_realloc(*line, *len, *capacity);
		if (!tmp)
			return (0);
		*line = tmp;
	}
	(*line)[*len] = c;
	(*len)++;
	return (1);
}

char *read_line_noninteractive(void)
{
	char *line;
	char c;
	int len;
	int capacity;
	ssize_t ret;

	line = NULL;
	len = 0;
	capacity = 0;
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
			break;
		if (c == '\n')
			break;
		if (!append_char(&line, c, &len, &capacity))
			return (NULL);
	}
	if (len == 0 && ret <= 0)
		return (NULL);
	line = ft_realloc(line, len, len + 1);
	if (line)
		line[len] = '\0';
	return (line);
}
