/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:06:11 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 20:14:34 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "main_helpers.h"
#include <string.h>

static char *escape_export_value(const char *s)
{
	size_t i;
	size_t outlen;
	char *out;
	size_t j;

	if (!s)
		return (gc_strdup(""));
	i = 0;
	outlen = 0;
	while (s[i])
	{
		if (s[i] == '"' || s[i] == '\\')
			outlen++;
		outlen++;
		i++;
	}
	out = (char *)gc_malloc(outlen + 1);
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		if (s[i] == '"' || s[i] == '\\')
		{
			out[j] = '\\';
			j++;
		}
		out[j] = s[i];
		j++;
		i++;
	}
	out[j] = '\0';
	return (out);
}

static int env_count_vars(t_env_list *env)
{
	t_env_node *n;
	int c;

	c = 0;
	if (!env)
		return (0);
	n = env->head;
	while (n)
	{
		c++;
		n = n->next;
	}
	return (c);
}

static void sort_lines(char **lines, int n)
{
	int i;
	int j;
	char *tmp;

	i = 0;
	while (i < n)
	{
		j = i + 1;
		while (j < n)
		{
			if (ft_strcmp(lines[i], lines[j]) > 0)
			{
				tmp = lines[i];
				lines[i] = lines[j];
				lines[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static char *build_export_line(const char *key, const char *val)
{
	char *esc;
	size_t len;
	char *line;
	size_t i;
	size_t j;

	if (!key)
		return (NULL);
	if (val)
		esc = escape_export_value(val);
	else
		esc = NULL;
	len = 11 + ft_strlen(key);
	if (esc)
		len += 2 + ft_strlen(esc);
	line = (char *)gc_malloc(len + 1);
	if (!line)
		return (NULL);
	i = 0;
	j = 0;
	while ("declare -x "[i])
	{
		line[j] = "declare -x "[i];
		j++;
		i++;
	}
	i = 0;
	while (key[i])
	{
		line[j] = key[i];
		j++;
		i++;
	}
	if (esc)
	{
		line[j] = '=';
		j++;
		line[j] = '"';
		j++;
		i = 0;
		while (esc[i])
		{
			line[j] = esc[i];
			j++;
			i++;
		}
		line[j] = '"';
		j++;
	}
	line[j] = '\0';
	return (line);
}

void print_exported_env(void)
{
	int cap;
	int count;
	char **lines;
	t_env_node *n;
	int i;

	cap = env_count_vars(g_shell.env) + g_shell.exported_count;
	if (cap < 1)
		return;
	lines = (char **)gc_calloc((size_t)cap + 1, sizeof(char *));
	if (!lines)
		return;
	count = 0;
	n = NULL;
	if (g_shell.env)
		n = g_shell.env->head;
	while (n)
	{
		lines[count] = build_export_line(n->key, n->value);
		if (lines[count])
			count++;
		n = n->next;
	}
	i = 0;
	while (i < g_shell.exported_count)
	{
		int present;
		t_env_node *m;

		present = 0;
		m = g_shell.env ? g_shell.env->head : NULL;
		while (m)
		{
			if (m->key && g_shell.exported_vars[i] && ft_strcmp(m->key, g_shell.exported_vars[i]) == 0)
			{
				present = 1;
				break;
			}
			m = m->next;
		}
		if (!present && g_shell.exported_vars[i])
		{
			lines[count] = build_export_line(g_shell.exported_vars[i], NULL);
			if (lines[count])
				count++;
		}
		i++;
	}
	sort_lines(lines, count);
	i = 0;
	while (i < count)
	{
		if (lines[i])
		{
			ft_putstr_fd(lines[i], STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		i++;
	}
}

int ft_env(char **args)
{
	t_env_node *n;

	if (args[1])
	{
		ft_putstr_fd("env: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		g_shell.last_status = 127;
		return (127);
	}
	n = g_shell.env ? g_shell.env->head : NULL;
	while (n)
	{
		if (n->key)
		{
			ft_putstr_fd(n->key, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			if (n->value)
				ft_putstr_fd(n->value, STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		n = n->next;
	}
	return (0);
}

static void remove_from_exported(const char *name)
{
	int i;
	int j;

	i = 0;
	while (i < g_shell.exported_count)
	{
		if (g_shell.exported_vars[i] && ft_strcmp(g_shell.exported_vars[i], name) == 0)
		{
			j = i;
			while (j + 1 < g_shell.exported_count)
			{
				g_shell.exported_vars[j] = g_shell.exported_vars[j + 1];
				j++;
			}
			g_shell.exported_vars[g_shell.exported_count - 1] = NULL;
			g_shell.exported_count--;
			return;
		}
		i++;
	}
}

static void unset_key(const char *key)
{
	t_env_node *prev;
	t_env_node *cur;

	if (!g_shell.env || !key)
		return;
	prev = NULL;
	cur = g_shell.env->head;
	while (cur)
	{
		if (cur->key && ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				g_shell.env->head = cur->next;
			if (g_shell.env->tail == cur)
				g_shell.env->tail = prev;
			remove_from_exported(key);
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

int ft_unset(char **args)
{
	int i;
	int ret;

	i = 1;
	ret = 0;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
			i++;
			continue;
		}
		unset_key(args[i]);
		i++;
	}
	g_shell.last_status = ret;
	return (ret);
}
