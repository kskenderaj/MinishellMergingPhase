/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 17:42:17 by kskender         ###   ########.fr       */
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
	g_shell.env = env;
	exit_status = main_loop(env);
	gc_cleanup();
	free_env_list(env);
	return (exit_status);
}
