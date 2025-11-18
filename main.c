/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/18 01:00:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"
#include "executor.h"
#include "builtins.h"
#include "garbage_collector.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define PROMPT "minishell$ "

static char	**env_list_to_array(t_env_list *env)
{
	t_env_node	*node;
	char		**envp;
	char		*temp;
	int			i;

	envp = malloc(sizeof(char *) * (env->size + 1));
	if (!envp)
		return (NULL);
	node = env->head;
	i = 0;
	while (node)
	{
		temp = ft_strjoin(node->key, "=");
		if (!temp)
			return (ft_free_array(envp), NULL);
		envp[i] = ft_strjoin(temp, node->value);
		free(temp);
		if (!envp[i])
			return (ft_free_array(envp), NULL);
		node = node->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

static char	***cmdlist_to_array(t_cmd_list *cmdlst)
{
	char		***cmds;
	t_cmd_node	*node;
	int			i;

	cmds = malloc(sizeof(char **) * (cmdlst->size + 1));
	if (!cmds)
		return (NULL);
	node = cmdlst->head;
	i = 0;
	while (node)
	{
		cmds[i] = node->cmd;
		node = node->next;
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}

static int	handle_single_command(t_cmd_node *cmd, t_env_list *env)
{
	char	**envp;
	int		ret;
	int		saved_stdin;
	int		saved_stdout;
	int		in_fd;
	int		out_fd;

	if (!cmd)
		return (0);
	envp = env_list_to_array(env);
	if (!envp)
		return (1);
	
	// Handle file-based redirections if present
	saved_stdin = -1;
	saved_stdout = -1;
	in_fd = -1;
	out_fd = -1;
	
	if (cmd->files && cmd->files->head)
	{
		// Save original stdin/stdout
		saved_stdin = dup(STDIN_FILENO);
		saved_stdout = dup(STDOUT_FILENO);
		if (saved_stdin < 0 || saved_stdout < 0)
		{
			if (saved_stdin >= 0)
				close(saved_stdin);
			if (saved_stdout >= 0)
				close(saved_stdout);
			ft_free_array(envp);
			return (1);
		}
		
		// Setup redirections from files list
		// Save current last_status before redirections
		int saved_status = g_shell.last_status;
		in_fd = setup_input_file_from_cmd(cmd);
		out_fd = setup_output_file_from_cmd(cmd);
		
		// Check if redirection setup failed (indicated by g_shell.last_status = 1)
		if (g_shell.last_status == 1 && saved_status == 0)
		{
			// Redirection failed - restore FDs and return error
			if (saved_stdin >= 0)
			{
				dup2(saved_stdin, STDIN_FILENO);
				close(saved_stdin);
			}
			if (saved_stdout >= 0)
			{
				dup2(saved_stdout, STDOUT_FILENO);
				close(saved_stdout);
			}
			if (in_fd >= 0)
				close(in_fd);
			if (out_fd >= 0)
				close(out_fd);
			ft_free_array(envp);
			return (1);  // File error = EXIT 1
		}
		
		// Apply redirections
		if (in_fd >= 0 && in_fd != NO_REDIRECTION)
		{
			dup2(in_fd, STDIN_FILENO);
			close(in_fd);
		}
		if (out_fd >= 0 && out_fd != NO_REDIRECTION)
		{
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);
		}
	}
	if (!cmd->cmd || !cmd->cmd[0] || (cmd->cmd[0] && !cmd->cmd[0][0]))
	{
		if (saved_stdin >= 0)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout >= 0)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		ft_free_array(envp);
		g_shell.last_status = 0;
		return (0);  // EXIT 0, no stderr!
	}
	
	ret = table_of_builtins(cmd, envp, 1);
	if (ret != 128)
	{
		// Restore FDs after builtin
		if (saved_stdin >= 0)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout >= 0)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		ft_free_array(envp);
		return (ret);
	}
	
	ret = exec_pipeline(&(cmd->cmd), 1, envp);
	
	// Restore FDs after external command
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	
	ft_free_array(envp);
	return (ret);
}

static int	handle_pipeline(t_cmd_list *cmdlst, t_env_list *env)
{
	char	***cmds;
	char	**envp;
	int		ret;

	cmds = cmdlist_to_array(cmdlst);
	if (!cmds)
		return (1);
	envp = env_list_to_array(env);
	if (!envp)
		return (free(cmds), 1);
	ret = exec_pipeline(cmds, (int)cmdlst->size, envp);
	ft_free_array(envp);
	free(cmds);
	return (ret);
}

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

static int	process_input_line(char *line, t_env_list *env, int last_status)
{
	t_token_list	toklst;
	t_cmd_list		cmdlst;

	if (!line || !*line)
		return (last_status);
	init_token_lst(&toklst);
	init_cmd_lst(&cmdlst);
	if (tokenize(&toklst, line) != 0)
		return (2);
	if (token_to_cmd(&toklst, &cmdlst, env, last_status) != 0)
		return (2);
	if (cmdlst.syntax_error)
	{
		ft_putendl_fd("minishell: syntax error", 2);
		return (2);
	}
	if (cmdlst.head)
		return (process_command(&cmdlst, env));
	return (0);
}

static char	*read_line_noninteractive(void)
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
			capacity = capacity == 0 ? 64 : capacity * 2;
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

static int	main_loop(t_env_list *env)
{
	char	*line;
	int		last_status;
	int		interactive;

	last_status = 0;
	interactive = isatty(STDIN_FILENO);
	while (1)
	{
		if (interactive)
			line = readline(PROMPT);
		else
			line = read_line_noninteractive();
		if (!line)
			break ;
		if (*line)
		{
			if (interactive)
				add_history(line);
			last_status = process_input_line(line, env, last_status);
		}
		free(line);
	}
	return (last_status);
}

static t_env_list	*init_environment(char **envp)
{
	t_env_list	*env;

	env = malloc(sizeof(t_env_list));
	if (!env)
		return (NULL);
	init_env_lst(env);
	if (!get_envs(envp, env))
	{
		free_env_list(env);
		return (NULL);
	}
	return (env);
}

int	main(int argc, char **argv, char **envp)
{
	t_env_list	*env;
	int			exit_status;

	(void)argc;
	(void)argv;
	gc_init();
	init_shell();
	start_signals();
	env = init_environment(envp);
	if (!env)
	{
		gc_cleanup();
		return (1);
	}
	exit_status = main_loop(env);
	gc_cleanup();
	free_env_list(env);
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", 1);
	return (exit_status);
}
