/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 18:01:23 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static char	*create_env_string(t_env_node *node)
{
	char	*temp;
	char	*result;

	temp = ft_strjoin(node->key, "=");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, node->value);
	free(temp);
	return (result);
}

char	**env_list_to_array(t_env_list *env)
{
	t_env_node	*node;
	char		**envp;
	int			i;

	envp = malloc(sizeof(char *) * (env->size + 1));
	if (!envp)
		return (NULL);
	node = env->head;
	i = 0;
	while (node)
	{
		envp[i] = create_env_string(node);
		if (!envp[i])
			return (ft_free_array(envp), NULL);
		node = node->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

char	***cmdlist_to_array(t_cmd_list *cmdlst)
{
	char		***cmds;
	t_cmd_node	*node;
	int			i;

	cmds = malloc(sizeof(char **) * (cmdlst->size + 1));
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
