/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 16:04:59 by kskender          #+#    #+#             */
/*   Updated: 2025/11/15 21:24:55 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "builtins.h"

/* forward prototypes for helpers (dynamic arrays) */
static int create_pipes(int (*pipes)[2], int ncmds);
static int spawn_pipeline_children(char ***cmds, int ncmds, char **envp,
								   int (*pipes)[2], pid_t *pids);
static void setup_child_io_and_exec(int idx, int ncmds, int (*pipes)[2],
									int in_fd, int out_fd, char **cmd, char **envp);
static void close_all_pipes(int (*pipes)[2], int ncmds);
static int wait_children(pid_t *pids, int ncmds);

int exec_pipeline(char ***cmds, int ncmds, char **envp)
{
	int (*pipes)[2];
	pid_t *pids;

	if (ncmds <= 0)
	{
		g_shell.last_status = 1;
		return (1);
	}
	/* allocate only what we need */
	pipes = NULL;
	if (ncmds > 1)
	{
		pipes = (int (*)[2])malloc(sizeof(int[2]) * (ncmds - 1));
		if (!pipes)
		{
			g_shell.last_status = 1;
			return (1);
		}
	}
	pids = (pid_t *)malloc(sizeof(pid_t) * ncmds);
	if (!pids)
	{
		if (pipes)
			free(pipes);
		g_shell.last_status = 1;
		return (1);
	}
	if (create_pipes(pipes, ncmds) != 0)
	{
		if (pipes)
			close_all_pipes(pipes, ncmds);
		free(pids);
		if (pipes)
			free(pipes);
		g_shell.last_status = 1;
		return (1);
	}
	if (spawn_pipeline_children(cmds, ncmds, envp, pipes, pids) != 0)
	{
		if (pipes)
			close_all_pipes(pipes, ncmds);
		free(pids);
		if (pipes)
			free(pipes);
		g_shell.last_status = 1;
		return (1);
	}
	if (pipes)
		close_all_pipes(pipes, ncmds);
	g_shell.last_status = wait_children(pids, ncmds);
	free(pids);
	if (pipes)
		free(pipes);
	return (g_shell.last_status);
}

static int create_pipes(int (*pipes)[2], int ncmds)
{
	int i;
	if (ncmds <= 1 || !pipes)
		return (0);
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
								   int (*pipes)[2], pid_t *pids)
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
			setup_child_io_and_exec(i, ncmds, pipes, in_fd, out_fd, cmds[i], envp);
		}
		if (in_fd != -1)
			close(in_fd);
		if (out_fd != -1)
			close(out_fd);
		i++;
	}
	return (0);
}

/* helpers to keep functions short and norm-compliant */
static void child_dup_io(int idx, int ncmds, int (*pipes)[2], int in_fd, int out_fd)
{
	if (idx > 0 && pipes)
		dup2(pipes[idx - 1][0], STDIN_FILENO);
	if (in_fd != -1)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (idx < ncmds - 1 && pipes)
		dup2(pipes[idx][1], STDOUT_FILENO);
	if (out_fd != -1)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
}

static void child_close_all_pipes(int (*pipes)[2], int ncmds)
{
	int j;
	if (!pipes)
		return;
	j = 0;
	while (j < ncmds - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
}

static void child_apply_envp(char **envp)
{
	int ei;
	char *eq;
	char *key;

	if (!envp)
		return;
	ei = 0;
	while (envp[ei])
	{
		eq = ft_strchr(envp[ei], '=');
		if (eq)
		{
			key = gc_substr(envp[ei], 0, (unsigned int)(eq - envp[ei]));
			if (key)
				setenv(key, eq + 1, 1);
		}
		ei++;
	}
}

static int run_child_builtin(char **cmd)
{
	if (!cmd || !cmd[0])
		return (-1);
	if (cmd[0][0] == '\0')
		_exit(0);
	if (!strcmp(cmd[0], "echo"))
		return (ft_echo(cmd));
	if (!strcmp(cmd[0], "cd"))
		return (ft_cd(cmd));
	if (!strcmp(cmd[0], "pwd"))
		return (ft_pwd(cmd));
	if (!strcmp(cmd[0], "export"))
		return (ft_export(cmd));
	if (!strcmp(cmd[0], "unset"))
		return (ft_unset(cmd));
	if (!strcmp(cmd[0], "env"))
		return (ft_env(cmd));
	if (!strcmp(cmd[0], "exit"))
		return (ft_exit(cmd));
	return (-1);
}

static void setup_child_io_and_exec(int idx, int ncmds, int (*pipes)[2],
									int in_fd, int out_fd, char **cmd, char **envp)
{
	int ret;
	child_dup_io(idx, ncmds, pipes, in_fd, out_fd);
	child_close_all_pipes(pipes, ncmds);
	child_apply_envp(envp);
	ret = run_child_builtin(cmd);
	if (ret != -1)
		_exit(ret);
	exec_external(cmd, envp);
	_exit(127);
}

static void close_all_pipes(int (*pipes)[2], int ncmds)
{
	int i;
	if (!pipes)
		return;
	i = 0;
	while (i < ncmds - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static int wait_children(pid_t *pids, int ncmds)
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
	{
		g_shell.last_status = WEXITSTATUS(status);
		return (g_shell.last_status);
	}
	if (WIFSIGNALED(status))
	{
		g_shell.last_status = 128 + WTERMSIG(status);
		return (g_shell.last_status);
	}
	g_shell.last_status = 128;
	return (g_shell.last_status);
}
