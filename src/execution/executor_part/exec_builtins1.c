/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:07:02 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 13:55:36 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static int cd_prepare_and_validate(char **args, int *argc, int *start_index)
{
	*start_index = 1;
	if (*argc > 1 && strcmp(args[1], "--") == 0)
	{
		*start_index = 2;
		(*argc)--;
	}
	if (*argc > 2)
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

static int resolve_cd_target(char **args, int argc, char **target)
{
	int start_index;

	if (cd_prepare_and_validate(args, &argc, &start_index))
		return (1);
	if (argc == 1 || (args[start_index] && args[start_index][0] == '\0'))
	{
		*target = getenv("HOME");
		if (!*target)
			return (ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO), 1);
		return (0);
	}
	if (strcmp(args[start_index], "-") == 0)
	{
		*target = getenv("OLDPWD");
		if (!*target)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO);
			return (1);
		}
		return (0);
	}
	*target = args[start_index];
	return (0);
}

static int cd_print_and_update(char **args, int argc, char *oldpwd,
							   char *newpwd)
{
	if (args[1] && strcmp(args[1], "-") == 0)
		printf("%s\n", newpwd);
	else if (argc > 2 && strcmp(args[1], "--") == 0 && strcmp(args[2], "-") == 0)
		printf("%s\n", newpwd);
	setenv("OLDPWD", oldpwd, 1);
	setenv("PWD", newpwd, 1);
	return (0);
}

int ft_cd(char **args, t_shell_state *shell)
{
	char *target;
	char oldpwd[PATH_MAX];
	char newpwd[PATH_MAX];
	int argc;

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
	cd_print_and_update(args, argc, oldpwd, newpwd);
	update_shell_env("OLDPWD", oldpwd, shell);
	update_shell_env("PWD", newpwd, shell);
	shell->last_status = 0;
	return (0);
}
