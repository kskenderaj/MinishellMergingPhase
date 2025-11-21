/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 16:04:59 by kskender          #+#    #+#             */
/*   Updated: 2025/11/21 17:37:41 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* forward prototypes for helpers to avoid implicit declarations under C99 */
static int	create_pipes(int **pipes, int ncmds);
static int	spawn_pipeline_children(char ***cmds, int ncmds, char **envp,
				int **pipes, pid_t *pids, char ****per_cmd_envs);
static void	setup_child_io_and_exec(int idx, int ncmds, int **pipes, int in_fd,
				int out_fd, char **cmd, char **envp);
static void	close_all_pipes(int **pipes, int ncmds);
static int	wait_children(pid_t *pids, int ncmds);

int	exec_pipeline(char ***cmds, int ncmds, char **envp, char ****per_cmd_envs)
{
	int		**pipes;
	pid_t	*pids;
	int		ret;
	int		i;

	if (ncmds <= 0)
		return (1);
	pipes = gc_malloc(sizeof(int *) * (ncmds - 1));
	if (!pipes)
		return (1);
	i = 0;
	while (i < ncmds - 1)
	{
		pipes[i] = gc_malloc(sizeof(int) * 2);
		if (!pipes[i])
			return (1);
		i++;
	}
	pids = gc_malloc(sizeof(pid_t) * ncmds);
	if (!pids)
		return (1);
	if (create_pipes(pipes, ncmds) != 0)
		return (1);
	if (spawn_pipeline_children(cmds, ncmds, envp, pipes, pids,
			per_cmd_envs) != 0)
	{
		close_all_pipes(pipes, ncmds);
		return (1);
	}
	close_all_pipes(pipes, ncmds);
	ret = wait_children(pids, ncmds);
	return (ret);
}

static int	create_pipes(int **pipes, int ncmds)
{
	int	i;

	i = 0;
	while (i < ncmds - 1)
	{
		if (pipe(pipes[i]) == -1)
			return (1);
		i++;
	}
	return (0);
}

static int	spawn_pipeline_children(char ***cmds, int ncmds, char **envp,
		int **pipes, pid_t *pids, char ****per_cmd_envs)
{
	int		i;
	int		in_fd;
	int		out_fd;
	int		redir_status;
	char	**cmd_envp;

	i = 0;
	while (i < ncmds)
	{
		in_fd = -1;
		out_fd = -1;
		redir_status = setup_redirections(cmds[i], &in_fd, &out_fd);
		if (redir_status != 0)
		{
			g_shell.last_status = redir_status;
			return (redir_status);
		}
		cmd_envp = (per_cmd_envs
				&& (*per_cmd_envs)[i]) ? (*per_cmd_envs)[i] : envp;
		pids[i] = fork();
		if (pids[i] == 0)
		{
			reset_signals_for_child();
			setup_child_io_and_exec(i, ncmds, pipes, in_fd, out_fd, cmds[i],
				cmd_envp);
		}
		if (in_fd != -1)
			close(in_fd);
		if (out_fd != -1)
			close(out_fd);
		i++;
	}
	return (0);
}

static void	setup_child_io_and_exec(int idx, int ncmds, int **pipes, int in_fd,
		int out_fd, char **cmd, char **envp)
{
	int		j;
	int		ei;
	char	*eq;
	char	*key;
	char	*val;

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
	if (envp)
	{
		ei = 0;
		while (envp[ei])
		{
			eq = ft_strchr(envp[ei], '=');
			if (eq)
			{
				key = gc_substr(envp[ei], 0, (unsigned int)(eq - envp[ei]));
				val = eq + 1;
				if (key)
					setenv(key, val, 1);
			}
			ei++;
		}
	}
	if (!cmd || !cmd[0] || !cmd[0][0])
	{
		exit(0);
	}
	if (!strcmp(cmd[0], "echo"))
	{
		ft_echo(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "cd"))
	{
		ft_cd(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "pwd"))
	{
		ft_pwd(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "export"))
	{
		ft_export(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "unset"))
	{
		ft_unset(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "env"))
	{
		ft_env(cmd);
		exit(0);
	}
	else if (!strcmp(cmd[0], "exit"))
	{
		ft_exit(cmd);
		exit(0);
	}
	exec_external(cmd, envp);
	_exit(127);
}

static void	close_all_pipes(int **pipes, int ncmds)
{
	int	i;

	i = 0;
	while (i < ncmds - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static int	wait_children(pid_t *pids, int ncmds)
{
	int	i;
	int	status;

	status = 0;
	/* Ignore signals while waiting for children - let child handle them */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	i = 0;
	while (i < ncmds)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	/* Restore interactive signal handlers */
	start_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (128);
}
