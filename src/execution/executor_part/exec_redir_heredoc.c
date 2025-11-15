/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_heredoc.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:28:18 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 21:24:50 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "parser.h"

// Counts heredoc redirections
int count_heredoc(t_commandlist *cmd)
{
	int count;
	t_filelist *cur;

	count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == HEREDOC)
			count++;
		cur = cur->next;
	}
	return (count);
}

// Finds the last heredoc
static t_filelist *find_last_heredoc(t_commandlist *cmd, int heredoc_count)
{
	int cur_count;
	t_filelist *cur;

	cur_count = 0;
	cur = cmd->files;
	while (cur)
	{
		if (cur->type == HEREDOC)
		{
			cur_count++;
			if (cur_count == heredoc_count)
				return (cur);
		}
		cur = cur->next;
	}
	return (NULL);
}

// helpers to keep norm <= 25 lines
static void normalize_delimiter(t_filelist *last_heredoc, char **norm, int *quoted)
{
	char *delim;
	size_t dl;

	*quoted = 0;
	delim = last_heredoc->filename;
	if (!delim)
	{
		*norm = "";
		return;
	}
	dl = ft_strlen(delim);
	if (dl >= 2 && ((delim[0] == '\'' && delim[dl - 1] == '\'') || (delim[0] == '"' && delim[dl - 1] == '"')))
	{
		*quoted = 1;
		*norm = ft_substr(delim, 1, dl - 2);
	}
	else
		*norm = delim;
}

static char *expand_heredoc_line(const char *line)
{
	int i;
	char *out;

	i = 0;
	out = ft_strdup("");
	while (line[i])
	{
		if (line[i] == '$')
		{
			int span = skip_var((char *)line + i);
			if (span <= 1)
			{
				char *tmp = ft_strjoin(out, "$");
				free(out);
				out = tmp;
				i++;
				continue;
			}
			{
				char *val = get_expand((char *)line, i, g_shell.last_status, g_shell.env);
				char *tmp = ft_strjoin(out, val ? val : "");
				free(out);
				out = tmp;
				i += span;
				continue;
			}
		}
		{
			char buf[2] = {line[i], '\0'};
			char *tmp = ft_strjoin(out, buf);
			free(out);
			out = tmp;
		}
		i++;
	}
	return (out);
}

static void free_delimiter_if_needed(char *norm, t_filelist *last_heredoc, int quoted)
{
	if (quoted && norm && norm != last_heredoc->filename)
		free(norm);
}

static void process_heredoc_loop(int tmpfd, char *delim_norm, int quoted)
{
	char *line;

	while (1)
	{
		line = readline("heredoc> ");
		if (g_sigint_status)
		{
			if (line)
				free(line);
			break;
		}
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", STDERR_FILENO);
			break;
		}
		if (ft_strcmp(line, delim_norm) == 0)
		{
			free(line);
			break;
		}
		if (quoted)
			ft_putendl_fd(line, tmpfd);
		else
		{
			char *out = expand_heredoc_line(line);
			ft_putendl_fd(out, tmpfd);
			free(out);
		}
		free(line);
	}
}

// Opens the last heredoc
static int write_heredoc_to_temp(int tmpfd, t_filelist *last_heredoc)
{
	char *delim_norm;
	int quoted;

	normalize_delimiter(last_heredoc, &delim_norm, &quoted);
	start_heredoc_signals();
	process_heredoc_loop(tmpfd, delim_norm, quoted);
	start_signals();
	lseek(tmpfd, 0, SEEK_SET);
	free_delimiter_if_needed(delim_norm, last_heredoc, quoted);
	return (tmpfd);
}

int setup_heredoc_fd(t_commandlist *cmd)
{
	int heredoc_count;
	t_filelist *last_heredoc;
	int tmpfd;
	char template[] = "/tmp/minishell_heredoc_XXXXXX";

	heredoc_count = count_heredoc(cmd);
	if (heredoc_count == 0)
		return (-1);
	last_heredoc = find_last_heredoc(cmd, heredoc_count);
	if (!last_heredoc)
		return (-1);
	tmpfd = mkstemp(template);
	if (tmpfd == -1)
		return (-1);
	// Unlink the file immediately, so it's cleaned up on close
	unlink(template);
	return (write_heredoc_to_temp(tmpfd, last_heredoc));
}

int setup_heredoc_from_cmdnode(t_cmd_node *cmd)
{
	t_commandlist tmp;

	if (!cmd || !cmd->files)
		return (0);
	tmp.files = (t_filelist *)cmd->files;
	if (count_heredoc(&tmp) == 0)
		return (0);
	return (setup_heredoc_fd(&tmp));
}
