/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:30:47 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:20:21 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* implementation intentionally moved to exec_redir_separated.c */

static int	check_syntax_error(char *tok, char **args, int i)
{
	if ((strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0 || strcmp(tok,
				"<") == 0 || strcmp(tok, "<<") == 0) && args[i + 1] == NULL)
	{
		ft_putstr_fd("minishell: syntax error near token `newline`\n",
			STDERR_FILENO);
		return (1);
	}
	return (0);
}

static int	process_operators(t_redir_data *data, char *tok, char **args,
		int *i, t_shell_state *shell)
{
	int	res;

	res = handle_attached_operators(data, tok, shell);
	if (res != 2)
		return (res);
	res = handle_separated_operators(args, i, data->in_fd, data->out_fd, shell);
	return (res);
}

static void	init_redir_data(t_redir_data *data, char **args, int *in_fd,
		int *out_fd)
{
	data->args = args;
	data->in_fd = in_fd;
	data->out_fd = out_fd;
}

int	setup_redirections(char **args, int *in_fd, int *out_fd,
		t_shell_state *shell)
{
	int				i;
	int				res;
	char			*tok;
	t_redir_data	data;

	*in_fd = -1;
	*out_fd = -1;
	i = 0;
	init_redir_data(&data, args, in_fd, out_fd);
	while (args[i])
	{
		tok = args[i];
		data.idx = &i;
		if (check_syntax_error(tok, args, i))
			return (1);
		res = process_operators(&data, tok, args, &i, shell);
		if (res != 2 && res != 0)
			return (1);
		if (res == 2)
			i++;
	}
	if ((*in_fd != -1 && *in_fd < 0) || (*out_fd != -1 && *out_fd < 0))
		return (1);
	return (0);
}
