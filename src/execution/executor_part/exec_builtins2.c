/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:31:20 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/24 14:47:27 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name || !(name[0] == '_' || (name[0] >= 'A' && name[0] <= 'Z')
			|| (name[0] >= 'a' && name[0] <= 'z')))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!(name[i] == '_' || (name[i] >= 'A' && name[i] <= 'Z')
				|| (name[i] >= 'a' && name[i] <= 'z') || (name[i] >= '0'
					&& name[i] <= '9')))
			return (0);
		i++;
	}
	return (1);
}

static int	export_no_value(char *arg, t_shell_state *shell)
{
	int	i;

	if (!is_valid_identifier(arg))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	i = 0;
	while (i < shell->exported_count && shell->exported_vars[i])
	{
		if (ft_strcmp(shell->exported_vars[i], arg) == 0)
			return (0);
		i++;
	}
	if (shell->exported_count < MAX_EXPORTED - 1)
	{
		shell->exported_vars[shell->exported_count] = gc_strdup_persistent(shell->gc,
				arg);
		if (shell->exported_vars[shell->exported_count])
		{
			shell->exported_count++;
			shell->exported_vars[shell->exported_count] = NULL;
		}
	}
	return (0);
}

static int	export_with_value(char *arg, t_shell_state *shell)
{
	char	*eq;
	char	*name;
	size_t	namelen;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (export_no_value(arg, shell));
	namelen = (size_t)(eq - arg);
	name = (char *)gc_malloc(shell->gc, namelen + 1);
	if (!name)
		return (1);
	memcpy(name, arg, namelen);
	name[namelen] = '\0';
	if (!is_valid_identifier(name))
		return (export_invalid_error(name, shell));
	set_export_value(name, eq + 1, shell);
	return (0);
}

int	ft_export(char **args, t_shell_state *shell)
{
	int	i;
	int	status;
	int	has_error;

	i = 1;
	has_error = 0;
	if (!args[1])
	{
		print_exported_env(shell);
		shell->last_status = 0;
		return (0);
	}
	while (args[i])
	{
		if (ft_strchr(args[i], '='))
			status = export_with_value(args[i], shell);
		else
			status = export_no_value(args[i], shell);
		if (status != 0)
			has_error = 1;
		i++;
	}
	shell->last_status = has_error;
	return (has_error);
}
