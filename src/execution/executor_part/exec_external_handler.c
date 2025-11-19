/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:34:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 20:17:22 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

// Executes external command (execve in current process)
void	exec_external(char **args, char **envp)
{
	char	*exec_path;

	if (!args || !args[0])
		return ;
	exec_path = find_in_path(args[0]);
	if (!exec_path)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		gc_free(exec_path);
		exit(127);
	}
	// Check if file exists
	if (access(exec_path, F_OK) != 0)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		gc_free(exec_path);
		exit(127); // File not found
	}
	// Check if executable
	if (access(exec_path, X_OK) != 0)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
		gc_free(exec_path);
		exit(126); // Permission denied
	}
	execve(exec_path, args, envp);
	perror(args[0]);
	gc_cleanup();
	exit(126);
}

// Sets up redirections for builtins and runs them
int	exec_builtin_with_redir(int (*builtin)(char **), char **args, int in_fd,
		int out_fd)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
	ret = builtin(args);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (ret);
}

static char	*expand_heredoc_line(char *line, int should_expand)
{
	t_segment_list	*seglst;
	char			*expanded;

	if (!should_expand || !line)
		return (line);
	seglst = gc_malloc(sizeof(*seglst));
	if (!seglst)
		return (line);
	init_segment_lst(seglst);
	if (!find_segment(seglst, line))
		return (line);
	expanded = segments_expand(seglst, g_shell.env, g_shell.last_status);
	if (!expanded)
		return (line);
	return (expanded);
}

static char	*expand_heredoc_content(char *content, int quoted)
{
	char	*line;
	char	*expanded;
	char	*result;
	int		i;
	int		start;

	if (!content || quoted)
		return (content);
	result = gc_strdup("");
	i = 0;
	start = 0;
	while (content[i])
	{
		if (content[i] == '\n')
		{
			line = gc_substr(content, start, i - start);
			expanded = expand_heredoc_line(line, 1);
			result = gc_strjoin(result, expanded);
			result = gc_strjoin(result, "\n");
			start = i + 1;
		}
		i++;
	}
	if (start < i)
	{
		line = gc_substr(content, start, i - start);
		expanded = expand_heredoc_line(line, 1);
		result = gc_strjoin(result, expanded);
	}
	return (result);
}

int	exec_heredoc_from_content(char *content, int quoted)
{
	int		pipefd[2];
	char	*expanded;

	if (pipe(pipefd) == -1)
		return (-1);
	if (!content)
	{
		close(pipefd[1]);
		return (pipefd[0]);
	}
	expanded = expand_heredoc_content(content, quoted);
	write(pipefd[1], expanded, ft_strlen(expanded));
	close(pipefd[1]);
	return (pipefd[0]);
}

static char	*read_line_from_stdin(void)
{
	char	buffer[2];
	char	*line;
	char	*tmp;
	ssize_t	bytes;

	line = gc_strdup("");
	buffer[1] = '\0';
	while (1)
	{
		bytes = read(STDIN_FILENO, buffer, 1);
		if (bytes <= 0)
			return (NULL);
		if (buffer[0] == '\n')
			return (line);
		tmp = line;
		line = gc_strjoin(tmp, buffer);
	}
}

int	exec_heredoc(const char *delimiter, int quoted)
{
	char	*buffer;
	int		pipefd[2];
	char	*expanded;
	size_t	delim_len;
	int		is_tty;

	if (pipe(pipefd) == -1)
		return (-1);
	delim_len = ft_strlen(delimiter);
	// Handle empty delimiter - should never match (can't close heredoc)
	if (delim_len == 0)
		delim_len = (size_t)-1; // Set to max value so it never matches
	is_tty = isatty(STDIN_FILENO);
	if (is_tty)
		start_heredoc_signals();
	while (1)
	{
		if (is_tty)
			buffer = readline("> ");
		else
			buffer = read_line_from_stdin();
		// Check for Ctrl+C FIRST before checking EOF
		if (is_tty && g_sigint_status == 130)
		{
			if (buffer)
				free(buffer); // Free readline buffer if exists
			close(pipefd[1]);
			close(pipefd[0]);
			return (-2); // Exit heredoc due to Ctrl+C
		}
		// Now check for EOF (Ctrl+D or end of input)
		if (!buffer)
		{
			if (is_tty)
				write(STDERR_FILENO,
					"warning: here-document delimited by end-of-file\n", 49);
			close(pipefd[1]);
			return (pipefd[0]); // Return what we have so far
		}
		if (ft_strncmp(buffer, delimiter, delim_len) == 0
			&& buffer[delim_len] == '\0')
		{
			if (is_tty)
				free(buffer); // Free readline buffer for delimiter line
			break ;
		}
		expanded = expand_heredoc_line(buffer, !quoted);
		write(pipefd[1], expanded, ft_strlen(expanded));
		write(pipefd[1], "\n", 1);
		if (is_tty)
			free(buffer); // Free readline buffer after use
	}
	if (is_tty)
		start_signals();
	close(pipefd[1]);
	return (pipefd[0]);
}
