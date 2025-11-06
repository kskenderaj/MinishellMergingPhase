/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:31:20 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/05 21:18:20 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static char *strip_quotes(const char *value)
{
	int len;
	char *stripped;

	len = ft_strlen(value);
	if (len >= 2 && ((value[0] == '"' && value[len - 1] == '"') || (value[0] == '\'' && value[len - 1] == '\'')))
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

int is_valid_identifier(const char *name)
{
	int i;

	if (!name || !(name[0] == '_' || (name[0] >= 'A' && name[0] <= 'Z') || (name[0] >= 'a' && name[0] <= 'z')))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!(name[i] == '_' || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z') || (name[i] >= '0' && name[i] <= '9')))
			return (0);
		i++;
	}
	return (1);
}

static int is_in_exported(const char *name)
{
	int i;

	i = 0;
	while (i < g_shell.exported_count)
	{
		if (ft_strcmp(g_shell.exported_vars[i], name) == 0)
			return (1);
		i++;
	}
	return (0);
}

static void export_no_value(char *arg)
{
	if (!is_valid_identifier(arg))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	else
	{
		if (!is_in_exported(arg) && g_shell.exported_count < MAX_EXPORTED)
			g_shell.exported_vars[g_shell.exported_count++] = gc_strdup(arg);
	}
}

static void export_with_value(char *arg)
{
	char *eq;
	char *value;
	char *stripped;
	char *name;
	size_t namelen;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		/* no '=', behave like export no value */
		export_no_value(arg);
		return;
	}
	namelen = (size_t)(eq - arg);
	name = (char *)gc_malloc(namelen + 1);
	if (!name)
		return;
	memcpy(name, arg, namelen);
	name[namelen] = '\0';

	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("export: not a valid identifier: ", STDERR_FILENO);
		ft_putstr_fd(name, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	else
	{
		value = eq + 1;
		stripped = strip_quotes(value);
		if (stripped)
		{
			setenv(name, stripped, 1);
			gc_free(stripped);
		}
		else
			setenv(name, value, 1);
		/* track the name as exported */
		if (!is_in_exported(name) && g_shell.exported_count < MAX_EXPORTED)
			g_shell.exported_vars[g_shell.exported_count++] = gc_strdup(name);
	}
}

int ft_export(char **args)
{
	int i;

	i = 1;
	if (!args[1])
	{
		print_exported_env();
		return (0);
	}
	while (args[i])
	{
		if (ft_strchr(args[i], '='))
			export_with_value(args[i]);
		else
			export_no_value(args[i]);
		i++;
	}
	return (0);
}
