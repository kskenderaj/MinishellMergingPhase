/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:07:02 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 20:37:02 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/* resolve_cd_target: set *target based on args and argc
   returns 0 on success, 1 on error (and prints an error message) */
static int resolve_cd_target(char **args, int argc, char **target)
{
	if (argc > 2)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (argc == 1 || (args[1] && args[1][0] == '\0'))
	{
		*target = getenv("HOME");
		if (!*target)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			return (1);
		}
		return (0);
	}
	if (strcmp(args[1], "-") == 0)
	{
		*target = getenv("OLDPWD");
		if (!*target)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
			return (1);
		}
		return (0);
	}
	*target = args[1];
	return (0);
}

int ft_cd(char **args)
{
	char *target;
	char oldpwd[PATH_MAX];
	char newpwd[PATH_MAX];
	int argc;

	argc = 0;
	while (args[argc])
		argc++;
	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return (perror("minishell: cd: getcwd (oldpwd)"), 1);
	/* determine target and validate args */
	if (resolve_cd_target(args, argc, &target))
		return (1);
	if (chdir(target) != 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(target, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd((char *)strerror(errno), STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	if (!getcwd(newpwd, sizeof(newpwd)))
		return (perror("minishell: cd: getcwd (newpwd)"), 1);
	if (args[1] && strcmp(args[1], "-") == 0)
		printf("%s\n", newpwd);
	setenv("OLDPWD", oldpwd, 1);
	setenv("PWD", newpwd, 1);
	return (0);
}
