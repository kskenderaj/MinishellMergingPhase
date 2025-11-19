/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/19 17:42:46 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static char	***build_per_cmd_envs(t_cmd_list *cmdlst, char **envp)
{
	char		***per_cmd_envs;
	t_cmd_node	*node;
	int			i;

	per_cmd_envs = gc_malloc(sizeof(char **) * cmdlst->size);
	if (!per_cmd_envs)
		return (NULL);
	i = 0;
	node = cmdlst->head;
	while (node && i < (int)cmdlst->size)
	{
		if (node->env && node->env->size > 0)
			per_cmd_envs[i] = merge_env_arrays(envp, node->env);
		else
			per_cmd_envs[i] = NULL;
		node = node->next;
		i++;
	}
	return (per_cmd_envs);
}

static void	cleanup_per_cmd_envs(char ***per_cmd_envs, int size, char **envp)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (per_cmd_envs[i] && per_cmd_envs[i] != envp)
			ft_free_array(per_cmd_envs[i]);
		i++;
	}
}

int	handle_pipeline(t_cmd_list *cmdlst, t_env_list *env)
{
	char	***cmds;
	char	**envp;
	char	***per_cmd_envs;
	int		ret;

	cmds = cmdlist_to_array(cmdlst);
	if (!cmds)
		return (1);
	envp = env_list_to_array(env);
	if (!envp)
		return (free(cmds), 1);
	per_cmd_envs = build_per_cmd_envs(cmdlst, envp);
	if (!per_cmd_envs)
		return (ft_free_array(envp), free(cmds), 1);
	ret = exec_pipeline(cmds, (int)cmdlst->size, envp, &per_cmd_envs);
	cleanup_per_cmd_envs(per_cmd_envs, (int)cmdlst->size, envp);
	ft_free_array(envp);
	free(cmds);
	return (ret);
}
