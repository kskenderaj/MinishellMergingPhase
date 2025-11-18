/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/18 00:00:00 by jtoumani         ###   ########.fr       */
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

char **env_list_to_array(t_env_list *env)
{
	t_env_node *node;
	char **envp;
	char *temp;
	int i;

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

int execute_simple_command(t_cmd_node *cmd, t_env_list *env)
{
	char **envp;
	pid_t pid;
	int status;
	int ret;

	if (!cmd || !cmd->cmd || !cmd->cmd[0])
		return (0);
	
	envp = env_list_to_array(env);
	if (!envp)
		return (1);
	
	// Try builtin first
	ret = table_of_builtins(cmd, envp, 1);
	if (ret != 128)  // Was a builtin
	{
		ft_free_array(envp);
		return (ret);
	}
	
	// External command - fork and exec
	pid = fork();
	if (pid == 0)  // Child
	{
		exec_external(cmd->cmd, envp);
		exit(127);  // Should never reach here
	}
	else if (pid > 0)  // Parent
	{
		waitpid(pid, &status, 0);
		ft_free_array(envp);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		return (1);
	}
	ft_free_array(envp);
	return (1);
}

int process_command(t_cmd_list *cmdlst, t_env_list *envlst)
{
	t_cmd_node *cmd;
	int last_status;

	if (!cmdlst || !cmdlst->head)
		return (0);
	
	cmd = cmdlst->head;
	last_status = 0;
	
	// For now, just execute the first command (no pipes yet)
	while (cmd)
	{
		last_status = execute_simple_command(cmd, envlst);
		cmd = cmd->next;
	}
	
	return (last_status);
}

int main(int argc, char **argv, char **envp)
{
    char		*line;
    t_env_list	*env;
    t_token_list toklst;
    t_cmd_list	cmdlst;
    int			last_status;

    (void)argc;
    (void)argv;
    
    // Initialize garbage collector and shell
    gc_init();
    init_shell();
    start_signals();
    
    // Setup environment list (using malloc for persistent data)
    env = malloc(sizeof(t_env_list));
    if (!env)
    {
        gc_cleanup();
        return (1);
    }
    init_env_lst(env);
    if (!get_envs(envp, env))
    {
        free_env_list(env);
        gc_cleanup();
        return (1);
    }
    
    last_status = 0;
    
    // Main REPL loop
    while (1)
    {
        line = readline(PROMPT);
        if (!line)
            break;
        
        if (*line)
            add_history(line);
        
        // Initialize lists for this command
        init_token_lst(&toklst);
        init_cmd_lst(&cmdlst);
        
        // Tokenize input
        if (tokenize(&toklst, line) == 0)
        {
            // Convert tokens to commands
            if (token_to_cmd(&toklst, &cmdlst, env, last_status) == 0)
            {
                if (!cmdlst.syntax_error && cmdlst.head)
                {
                    // Execute commands
                    last_status = process_command(&cmdlst, env);
                }
                else if (cmdlst.syntax_error)
                {
                    ft_putendl_fd("minishell: syntax error", 2);
                    last_status = 2;
                }
            }
        }
        
        free(line);
        // Don't cleanup GC here - let it accumulate and cleanup at the end
    }
    
    // Cleanup and exit
    gc_cleanup();  // Clean all GC-allocated memory once at the end
    free_env_list(env);  // Free persistent env list AFTER gc_cleanup
    ft_putendl_fd("exit", 1);
    return (last_status);
}