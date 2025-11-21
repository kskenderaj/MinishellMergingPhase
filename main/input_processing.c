/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_processing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/21 17:52:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	process_command(t_cmd_list *cmdlst, t_env_list *envlst)
{
	int	ret;

	if (!cmdlst || !cmdlst->head)
		return (0);
	if (cmdlst->size == 1)
		ret = handle_single_command(cmdlst->head, envlst);
	else
		ret = handle_pipeline(cmdlst, envlst);
	g_shell.last_status = ret;
	return (ret);
}

int	process_input_line(char *line, t_env_list *env, int last_status)
{
	t_token_list	toklst;
	t_cmd_list		cmdlst;

	if (!line || !*line)
		return (last_status);
	init_token_lst(&toklst);
	init_cmd_lst(&cmdlst);
	if (tokenize(&toklst, line) != 0)
	{
		ft_putendl_fd("minishell: syntax error", 2);
		return (2);
	}
	if (token_to_cmd(&toklst, &cmdlst, env, last_status) != 0)
	{
		if (cmdlst.syntax_error)
			ft_putendl_fd("minishell: syntax error", 2);
		return (2);
	}
	if (cmdlst.syntax_error)
	{
		ft_putendl_fd("minishell: syntax error", 2);
		gc_clear();
		return (2);
	}
	process_all_heredocs(&cmdlst);
	if (cmdlst.head)
	{
		last_status = process_command(&cmdlst, env);
		gc_clear();
		return (last_status);
	}
	gc_clear();
	return (0);
}

char	*read_line_noninteractive(void)
{
	char	*line;
	char	c;
	int		len;
	int		capacity;
	ssize_t	ret;

	line = NULL;
	len = 0;
	capacity = 0;
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
			return (line);
		if (c == '\n')
			break ;
		if (len >= capacity)
		{
			capacity = (capacity == 0) * 64 + (capacity != 0) * (capacity * 2);
			line = ft_realloc(line, len, capacity);
			if (!line)
				return (NULL);
		}
		line[len++] = c;
	}
	if (len == 0 && ret <= 0)
		return (NULL);
	line = ft_realloc(line, len, len + 1);
	if (line)
		line[len] = '\0';
	return (line);
}
