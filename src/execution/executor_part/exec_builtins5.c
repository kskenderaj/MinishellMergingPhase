/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins5.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:10:37 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 16:10:56 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

int	is_in_exported(const char *name, t_shell_state *shell)
{
	int	i;

	if (!name)
		return (0);
	i = 0;
	while (i < shell->exported_count)
	{
		if (shell->exported_vars[i]
			&& ft_strcmp(shell->exported_vars[i], name) == 0)
			return (1);
		i++;
	}
	return (0);
}

int	export_invalid_error(char *name, t_shell_state *shell)
{
	(void)shell;
	ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
	ft_putstr_fd(name, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
	return (1);
}

void	set_export_value(char *name, char *value, t_shell_state *shell)
{
	char	*stripped;

	stripped = strip_quotes(value, shell);
	if (stripped)
	{
		setenv(name, stripped, 1);
		update_shell_env(name, stripped, shell);
		gc_free(shell->gc, stripped);
	}
	else
	{
		setenv(name, value, 1);
		update_shell_env(name, value, shell);
	}
	if (!is_in_exported(name, shell) && shell->exported_count < MAX_EXPORTED)
	{
		shell->exported_vars[shell->exported_count] = gc_strdup(shell->gc,
				name);
		if (shell->exported_vars[shell->exported_count])
			shell->exported_count++;
	}
}

static void	remove_from_exported(const char *name, t_shell_state *shell)
{
	int	i;
	int	j;

	i = 0;
	while (i < shell->exported_count)
	{
		if (shell->exported_vars[i]
			&& ft_strcmp(shell->exported_vars[i], name) == 0)
		{
			j = i;
			while (j < shell->exported_count - 1)
			{
				shell->exported_vars[j] = shell->exported_vars[j + 1];
				j++;
			}
			shell->exported_count--;
			return ;
		}
		i++;
	}
}

int	ft_unset(char **args, t_shell_state *shell)
{
	int	i;
	int	has_error;

	i = 1;
	has_error = 0;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("unset: not a valid identifier: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putchar_fd('\n', STDERR_FILENO);
			has_error = 1;
			i++;
			continue ;
		}
		unsetenv(args[i]);
		remove_from_env_list(shell->env, args[i]);
		remove_from_exported(args[i], shell);
		i++;
	}
	shell->last_status = has_error;
	return (has_error);
}
