/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/24 13:57:12 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	increment_shlvl(void)
{
	char	*shlvl_str;
	int		shlvl;
	char	new_shlvl[32];

	shlvl_str = getenv("SHLVL");
	if (!shlvl_str)
		shlvl = 0;
	else
		shlvl = ft_atoi(shlvl_str);
	shlvl++;
	snprintf(new_shlvl, sizeof(new_shlvl), "%d", shlvl);
	setenv("SHLVL", new_shlvl, 1);
}

static t_env_list	*init_environment(char **envp)
{
	t_env_list	*env;
	extern char	**environ;

	(void)envp;
	increment_shlvl();
	env = malloc(sizeof(t_env_list));
	if (!env)
		return (NULL);
	init_env_lst(env);
	if (!get_envs(environ, env))
	{
		free(env);
		return (NULL);
	}
	return (env);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell_state	shell;
	t_env_list		*env;
	int				exit_status;

	(void)argc;
	(void)argv;
	ft_memset(&shell, 0, sizeof(t_shell_state));
	shell.gc = gc_init();
	if (!shell.gc)
		return (1);
	init_shell(&shell);
	start_signals();
	env = init_environment(envp);
	if (!env)
	{
		gc_cleanup(shell.gc);
		return (1);
	}
	shell.env = env;
	exit_status = main_loop(env, &shell);
	free_env_list(env);
	gc_cleanup(shell.gc);
	return (exit_status);
}
