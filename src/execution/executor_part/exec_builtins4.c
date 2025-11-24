/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins4.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 16:10:09 by kskender          #+#    #+#             */
/*   Updated: 2025/11/24 14:28:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

void	update_shell_env(const char *name, const char *value,
		t_shell_state *shell)
{
	t_env_node	*existing;
	t_env_node	*new_node;

	if (!shell->env || !name)
		return ;
	existing = shell->env->head;
	while (existing)
	{
		if (ft_strcmp(existing->key, name) == 0)
		{
			if (existing->value)
				free(existing->value);
			existing->value = ft_strdup(value);
			return ;
		}
		existing = existing->next;
	}
	new_node = malloc(sizeof(t_env_node));
	if (new_node)
	{
		new_node->key = ft_strdup(name);
		new_node->value = ft_strdup(value);
		push_env(shell->env, new_node);
	}
}

char	*strip_quotes(const char *value, t_shell_state *shell)
{
	int		len;
	char	*stripped;

	len = ft_strlen(value);
	if (len >= 2 && ((value[0] == '"' && value[len - 1] == '"')
			|| (value[0] == '\'' && value[len - 1] == '\'')))
	{
		stripped = (char *)gc_malloc(shell->gc, (size_t)len - 1);
		if (stripped)
		{
			ft_strlcpy(stripped, value + 1, (size_t)len - 1);
			stripped[len - 2] = '\0';
		}
		return (stripped);
	}
	return (NULL);
}

void	print_exported_env(t_shell_state *shell)
{
	t_env_node	*node;
	int			i;

	node = shell->env->head;
	while (node)
	{
		if (!node->key)
		{
			node = node->next;
			continue ;
		}
		if (node->value && node->value[0])
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(node->key, STDOUT_FILENO);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(node->value, STDOUT_FILENO);
			ft_putstr_fd("\"\n", STDOUT_FILENO);
		}
		else
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(node->key, STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		node = node->next;
	}
	i = 0;
	while (i < shell->exported_count && i < MAX_EXPORTED)
	{
		if (shell->exported_vars[i] && shell->exported_vars[i][0] != '\0'
			&& is_valid_identifier(shell->exported_vars[i])
			&& !get_env_value(shell->env, shell->exported_vars[i]))
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(shell->exported_vars[i], STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		i++;
	}
}

int	ft_env(char **args, t_shell_state *shell)
{
	extern char	**environ;
	int			i;

	i = 0;
	(void)args;
	(void)shell;
	while (environ[i])
	{
		ft_putstr_fd(environ[i], STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		i++;
	}
	return (0);
}
