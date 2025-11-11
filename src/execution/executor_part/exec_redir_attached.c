/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_attached.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 02:36:12 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 02:36:12 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int handle_attached_append(char **args, int *idx, int *out_fd,
								  char *tok)
{
	char *fname;

	if (tok[2] != '\0')
		fname = tok + 2;
	else
		fname = args[*idx + 1];
	if (!fname)
		return (2);
	if (!strcmp(fname, ">") || !strcmp(fname, "<") || !strcmp(fname, ">>") || !strcmp(fname, "<<"))
		return (2);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (*out_fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(fname, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd((char *)strerror(errno), STDERR_FILENO);
		return (1);
	}
	if (tok[2] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

static int handle_attached_heredoc(char **args, int *idx, int *in_fd,
								   char *tok)
{
	char *delim;

	if (tok[2] != '\0')
		delim = tok + 2;
	else
		delim = args[*idx + 1];
	if (!delim)
		return (2);
	if (!strcmp(delim, ">") || !strcmp(delim, "<") || !strcmp(delim, ">>") || !strcmp(delim, "<<"))
		return (2);
	if (*in_fd != -1)
		close(*in_fd);
	*in_fd = exec_heredoc(delim);
	if (tok[2] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}
static int handle_attached_outfile(char **args, int *idx, int *out_fd,
								   char *tok)
{
	char *fname;

	if (tok[1] != '\0')
		fname = tok + 1;
	else
		fname = args[*idx + 1];
	if (!fname)
		return (2);
	if (!strcmp(fname, ">") || !strcmp(fname, "<") || !strcmp(fname, ">>") || !strcmp(fname, "<<"))
		return (2);
	if (*out_fd != -1)
		close(*out_fd);
	*out_fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (*out_fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(fname, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd((char *)strerror(errno), STDERR_FILENO);
		return (1);
	}
	if (tok[1] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

static int handle_attached_infile(char **args, int *idx, int *in_fd, char *tok)
{
	char *fname;

	if (tok[1] != '\0')
		fname = tok + 1;
	else
		fname = args[*idx + 1];
	if (!fname)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
		ft_putstr_fd(tok, STDERR_FILENO);
		ft_putstr_fd("`\n", STDERR_FILENO);
		return (2);
	}
	if (!strcmp(fname, ">") || !strcmp(fname, "<") || !strcmp(fname, ">>") || !strcmp(fname, "<<"))
		return (2);
	if (*in_fd != -1)
		close(*in_fd);
	*in_fd = open(fname, O_RDONLY);
	if (*in_fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(fname, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd((char *)strerror(errno), STDERR_FILENO);
		return (1);
	}
	if (tok[1] != '\0')
		shift_left_by(args, *idx, 1);
	else
		shift_left_by(args, *idx, 2);
	return (0);
}

int handle_attached_operators(t_redir_data *data, char *tok)
{
	if (strncmp(tok, ">>", 2) == 0)
		return (handle_attached_append(data->args, data->idx, data->out_fd,
									   tok));
	if (strncmp(tok, "<<", 2) == 0)
		return (handle_attached_heredoc(data->args, data->idx, data->in_fd,
										tok));
	if (tok[0] == '>' && tok[1] != '>')
		return (handle_attached_outfile(data->args, data->idx, data->out_fd,
										tok));
	if (tok[0] == '<' && tok[1] != '<')
		return (handle_attached_infile(data->args, data->idx, data->in_fd,
									   tok));
	return (2);
}
