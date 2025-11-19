/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:31:20 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 14:10:41 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

/* Helper to update shell's internal environment list */
void	update_shell_env(const char *name, const char *value)
{
	t_env_node	*existing;
	t_env_node	*new_node;

	if (!g_shell.env || !name)
		return ;
	/* Check if variable already exists and update it */
	existing = g_shell.env->head;
	while (existing)
	{
		if (ft_strcmp(existing->key, name) == 0)
		{
			/* Update existing value */
			if (existing->value)
				free(existing->value);
			existing->value = ft_strdup(value);
			return ;
		}
		existing = existing->next;
	}
	/* Variable doesn't exist, add new entry */
	new_node = malloc(sizeof(t_env_node));
	if (new_node)
	{
		new_node->key = ft_strdup(name);
		new_node->value = ft_strdup(value);
		push_env(g_shell.env, new_node);
	}
}

static char	*strip_quotes(const char *value)
{
	int		len;
	char	*stripped;

	len = ft_strlen(value);
	if (len >= 2 && ((value[0] == '"' && value[len - 1] == '"')
			|| (value[0] == '\'' && value[len - 1] == '\'')))
	{
		stripped = (char *)gc_malloc((size_t)len - 1);
		if (stripped)
		{
			/* copy inner content, ensure null-termination */
			ft_strlcpy(stripped, value + 1, (size_t)len - 1);
			stripped[len - 2] = '\0';
		}
		return (stripped);
	}
	return (NULL);
}

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

static int	is_in_exported(const char *name)
{
	int	i;

	i = 0;
	while (i < g_shell.exported_count)
	{
		if (ft_strcmp(g_shell.exported_vars[i], name) == 0)
			return (1);
		i++;
	}
	return (0);
}

static int	export_no_value(char *arg)
{
	if (!is_valid_identifier(arg))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	else
	{
		if (!is_in_exported(arg) && g_shell.exported_count < MAX_EXPORTED)
			g_shell.exported_vars[g_shell.exported_count++] = gc_strdup(arg);
	}
	return (0);
}

static int	export_with_value(char *arg)
{
	char	*eq;
	char	*value;
	char	*stripped;
	char	*name;
	size_t	namelen;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		/* no '=', behave like export no value */
		return (export_no_value(arg));
	}
	namelen = (size_t)(eq - arg);
	name = (char *)gc_malloc(namelen + 1);
	if (!name)
		return (1);
	memcpy(name, arg, namelen);
	name[namelen] = '\0';
	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
		ft_putstr_fd(name, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	else
	{
		value = eq + 1;
		stripped = strip_quotes(value);
		if (stripped)
		{
			setenv(name, stripped, 1);
			/* Also update shell's internal env list for variable expansion */
			update_shell_env(name, stripped);
			gc_free(stripped);
		}
		else
		{
			setenv(name, value, 1);
			/* Also update shell's internal env list for variable expansion */
			update_shell_env(name, value);
		}
		/* track the name as exported */
		if (!is_in_exported(name) && g_shell.exported_count < MAX_EXPORTED)
			g_shell.exported_vars[g_shell.exported_count++] = gc_strdup(name);
	}
	return (0);
}

int	ft_export(char **args)
{
	int	i;
	int	status;
	int	has_error;

	i = 1;
	has_error = 0;
	if (!args[1])
	{
		print_exported_env();
		g_shell.last_status = 0;
		return (0);
	}
	while (args[i])
	{
		if (ft_strchr(args[i], '='))
			status = export_with_value(args[i]);
		else
			status = export_no_value(args[i]);
		if (status != 0)
			has_error = 1;
		i++;
	}
	g_shell.last_status = has_error;
	return (has_error);
}
