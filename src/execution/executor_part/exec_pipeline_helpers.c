/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 16:04:59 by kskender          #+#    #+#             */
/*   Updated: 2025/11/06 03:38:10 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* forward prototypes for helpers to avoid implicit declarations under C99 */
static int create_pipes(int pipes[64][2], int ncmds);
static int spawn_pipeline_children(char ***cmds, int ncmds, char **envp,
								   int pipes[64][2], pid_t pids[64]);
static void setup_child_io_and_exec(int idx, int ncmds, int pipes[64][2],
									int in_fd, int out_fd, char **cmd, char **envp);
static void close_all_pipes(int pipes[64][2], int ncmds);
static int wait_children(pid_t pids[64], int ncmds);

int exec_pipeline(char ***cmds, int ncmds, char **envp)
{
	int pipes[64][2];
	pid_t pids[64];

	if (ncmds <= 0)
		return (1);
	if (create_pipes(pipes, ncmds) != 0)
		return (1);
	if (spawn_pipeline_children(cmds, ncmds, envp, pipes, pids) != 0)
	{
		close_all_pipes(pipes, ncmds);
		return (1);
	}
	close_all_pipes(pipes, ncmds);
	return (wait_children(pids, ncmds));
}

static int create_pipes(int pipes[64][2], int ncmds)
{
	int i;

	i = 0;
	while (i < ncmds - 1)
	{
		if (pipe(pipes[i]) == -1)
			return (1);
		i++;
	}
	return (0);
}

static int spawn_pipeline_children(char ***cmds, int ncmds, char **envp,
								   int pipes[64][2], pid_t pids[64])
{
	int i;
	int in_fd;
	int out_fd;

	i = 0;
	while (i < ncmds)
	{
		in_fd = -1;
		out_fd = -1;
		if (setup_redirections(cmds[i], &in_fd, &out_fd))
			return (1);
		pids[i] = fork();
		if (pids[i] == 0)
		{
			setup_child_io_and_exec(i, ncmds, pipes, in_fd, out_fd, cmds[i],
									envp);
		}
		if (in_fd != -1)
			close(in_fd);
		if (out_fd != -1)
			close(out_fd);
		i++;
	}
	return (0);
}

static void setup_child_io_and_exec(int idx, int ncmds, int pipes[64][2],
									int in_fd, int out_fd, char **cmd, char **envp)
{
	int j;

	if (idx > 0)
		dup2(pipes[idx - 1][0], STDIN_FILENO);
	if (in_fd != -1)
		dup2(in_fd, STDIN_FILENO);
	if (idx < ncmds - 1)
		dup2(pipes[idx][1], STDOUT_FILENO);
	if (out_fd != -1)
		dup2(out_fd, STDOUT_FILENO);
	j = 0;
	while (j < ncmds - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
	/* If a per-command envp was provided, apply it in this child so builtins
	 * see the modified environment. It's safe because we're in the child. */
	if (envp)
	{
		int ei = 0;
		while (envp[ei])
		{
			char *eq = ft_strchr(envp[ei], '=');
			if (eq)
			{
				char *key = gc_substr(envp[ei], 0, (unsigned int)(eq - envp[ei]));
				char *val = eq + 1;
				if (key)
					setenv(key, val, 1);
			}
			ei++;
		}
	}
	/* if command is a shell builtin, run it in this child process */
	if (cmd && cmd[0])
	{
		if (!strcmp(cmd[0], "echo"))
		{
			ft_echo(cmd);
			_exit(0);
		}
		else if (!strcmp(cmd[0], "cd"))
		{
			ft_cd(cmd);
			_exit(0);
		}
		else if (!strcmp(cmd[0], "pwd"))
		{
			ft_pwd(cmd);
			_exit(0);
		}
		else if (!strcmp(cmd[0], "export"))
		{
			ft_export(cmd);
			_exit(0);
		}
		else if (!strcmp(cmd[0], "unset"))
		{
			ft_unset(cmd);
			_exit(0);
		}
		else if (!strcmp(cmd[0], "env"))
		{
			ft_env(cmd);
			_exit(0);
		}
	}
	exec_external(cmd, envp);
	_exit(127);
}

static void close_all_pipes(int pipes[64][2], int ncmds)
{
	int i;

	i = 0;
	while (i < ncmds - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static int wait_children(pid_t pids[64], int ncmds)
{
	int i;
	int status;

	status = 0;
	i = 0;
	while (i < ncmds)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (128);
}
