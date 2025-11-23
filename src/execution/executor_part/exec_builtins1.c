/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:07:02 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:08:45 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

/* resolve_cd_target: set *target based on args and argc
   returns 0 on success, 1 on error (and prints an error message) */
static int	resolve_cd_target(char **args, int argc, char **target)
{
	if (argc > 2)
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (argc == 1 || (args[1] && args[1][0] == '\0'))
	{
		*target = getenv("HOME");
		if (!*target)
			return (ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO), 1);
		return (0);
	}
	if (strcmp(args[1], "-") == 0)
	{
		*target = getenv("OLDPWD");
		if (!*target)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO);
			return (1);
		}
		return (0);
	}
	*target = args[1];
	return (0);
}

/*
 * ft_cd - Change the current working directory.
 * Handles all edge cases:
 * - No argument: change to HOME
 * - "-": change to OLDPWD and print new dir
 * - Too many arguments: error
 * - Nonexistent directory: error
 * - Permission denied: error
 * - Updates OLDPWD and PWD in environment
 */
int	ft_cd(char **args, t_shell_state *shell)
{
	char	*target;
	char	oldpwd[PATH_MAX];
	char	newpwd[PATH_MAX];
	int		argc;

	argc = 0;
	while (args[argc])
		argc++;
	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return (perror("cd: getcwd (oldpwd)"), shell->last_status = 1, 1);
	if (resolve_cd_target(args, argc, &target))
		return (shell->last_status = 1, 1);
	if (chdir(target) != 0)
		return (perror("cd"), shell->last_status = 1, 1);
	if (!getcwd(newpwd, sizeof(newpwd)))
		return (perror("cd: getcwd (newpwd)"), shell->last_status = 1, 1);
	if (args[1] && strcmp(args[1], "-") == 0)
		printf("%s\n", newpwd);
	setenv("OLDPWD", oldpwd, 1);
	setenv("PWD", newpwd, 1);
	update_shell_env("OLDPWD", oldpwd, shell);
	update_shell_env("PWD", newpwd, shell);
	shell->last_status = 0;
	return (0);
}
