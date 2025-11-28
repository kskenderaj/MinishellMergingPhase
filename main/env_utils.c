/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 13:48:31 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

char	*get_env_value(t_env_list *env, const char *key)
{
	t_env_node	*node;

	if (!env || !key)
		return (NULL);
	node = env->head;
	while (node)
	{
		if (ft_strcmp(node->key, key) == 0)
			return (node->value);
		node = node->next;
	}
	return (NULL);
}

static char	*create_env_string(t_env_node *node, t_shell_state *shell)
{
	char	*temp;
	char	*result;

	temp = gc_strjoin(shell->gc, node->key, "=");
	if (!temp)
		return (NULL);
	result = gc_strjoin(shell->gc, temp, node->value);
	return (result);
}

char	**env_list_to_array(t_env_list *env, t_shell_state *shell)
{
	t_env_node	*node;
	char		**envp;
	int			i;

	envp = gc_malloc(shell->gc, sizeof(char *) * (env->size + 1));
	if (!envp)
		return (NULL);
	node = env->head;
	i = 0;
	while (node)
	{
		envp[i] = create_env_string(node, shell);
		if (!envp[i])
			return (NULL);
		node = node->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

char	***cmdlist_to_array(t_cmd_list *cmdlst, t_shell_state *shell)
{
	char		***cmds;
	t_cmd_node	*node;
	int			i;

	cmds = gc_malloc(shell->gc, sizeof(char **) * (cmdlst->size + 1));
	if (!cmds)
		return (NULL);
	node = cmdlst->head;
	i = 0;
	while (node)
	{
		cmds[i] = node->cmd;
		node = node->next;
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}
