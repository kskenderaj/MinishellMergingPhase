/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_attached.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 23:59:26 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 16:06:51 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int	handle_attached_append(char **args, int *idx, int *out_fd,
		char *tok)
{
	char	*fname;

	if (tok[2] != '\0')
		fname = tok + 2;
	else
		fname = args[*idx + 1];
	if (!fname)
		return (1);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (tok[2] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

static int	handle_attached_heredoc(t_redir_data *redir, char *tok,
		t_shell_state *shell)
{
	char	*delim;

	if (tok[2] != '\0')
		delim = tok + 2;
	else
		delim = redir->args[*redir->idx + 1];
	if (!delim)
		return (1);
	if (*redir->in_fd != -1)
		close(*redir->in_fd);
	*redir->in_fd = exec_heredoc(delim, 0, shell);
	if (tok[2] != '\0')
		shift_left_by(redir->args, *redir->idx, 1);
	else
		shift_left_by(redir->args, *redir->idx, 2);
	return (0);
}

static int	handle_attached_outfile(char **args, int *idx, int *out_fd,
		char *tok)
{
	char	*fname;

	if (tok[1] != '\0')
		fname = tok + 1;
	else
		fname = args[*idx + 1];
	if (!fname)
		return (1);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (tok[1] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

static int	handle_attached_infile(char **args, int *idx, int *in_fd, char *tok)
{
	char	*fname;

	if (tok[1] != '\0')
		fname = tok + 1;
	else
		fname = args[*idx + 1];
	if (!fname)
		return (1);
	if (*in_fd != -1)
		close(*in_fd);
	*in_fd = open(fname, O_RDONLY);
	if (tok[1] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

int	handle_attached_operators(t_redir_data *data, char *tok,
		t_shell_state *shell)
{
	if (strncmp(tok, ">>", 2) == 0)
		return (handle_attached_append(data->args, data->idx, data->out_fd,
				tok));
	if (strncmp(tok, "<<", 2) == 0)
		return (handle_attached_heredoc(data, tok, shell));
	if (tok[0] == '>' && tok[1] != '>')
		return (handle_attached_outfile(data->args, data->idx, data->out_fd,
				tok));
	if (tok[0] == '<' && tok[1] != '<')
		return (handle_attached_infile(data->args, data->idx, data->in_fd,
				tok));
	return (2);
}
