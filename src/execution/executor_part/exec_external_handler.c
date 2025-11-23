/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:34:28 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:12:14 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static void	exec_error_and_exit(char *exec_path, int code,
		t_shell_state *shell)
{
	if (code == 127)
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
	else if (code == 126)
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
	else
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
	gc_free(shell->gc, exec_path);
	exit(code);
}

static int	check_exec_errors(char **args, char *exec_path,
		t_shell_state *shell)
{
	if (!exec_path)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		exec_error_and_exit(exec_path, 127, shell);
	}
	if (access(exec_path, F_OK) != 0)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		gc_free(shell->gc, exec_path);
		exit(127);
	}
	if (access(exec_path, X_OK) != 0)
	{
		ft_putstr_fd(args[0], STDERR_FILENO);
		exec_error_and_exit(exec_path, 126, shell);
	}
	return (0);
}

void	exec_external(char **args, char **envp, t_shell_state *shell)
{
	char	*exec_path;

	if (!args || !args[0])
		return ;
	reset_signals_for_child();
	exec_path = find_in_path(args[0], shell);
	check_exec_errors(args, exec_path, shell);
	execve(exec_path, args, envp);
	perror(args[0]);
	gc_cleanup(shell->gc);
	exit(126);
}

int	exec_heredoc(const char *delimiter, int quoted, t_shell_state *shell)
{
	int		pipefd[2];
	int		is_tty;

	if (pipe(pipefd) == -1)
		return (-1);
	is_tty = isatty(STDIN_FILENO);
	heredoc_read_loop(delimiter, quoted, pipefd, is_tty, shell);
	close(pipefd[1]);
	return (pipefd[0]);
}
