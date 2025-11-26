/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:18:05 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

int find_key(char *str, t_env_node *env)
{
	char *eq;
	size_t key_len;
	char *temp;

	if (!str || !env)
		return (0);
	eq = ft_strchr(str, '=');
	if (eq)
		key_len = (size_t)(eq - str);
	else
		key_len = ft_strlen(str);
	temp = ft_substr(str, 0, key_len);
	if (!temp)
		return (0);
	env->key = ft_strdup(temp);
	free(temp);
	if (!env->key)
		return (0);
	return (1);
}

int find_value(char *str, t_env_node *env)
{
	char *eq;
	size_t val_len;
	char *temp;

	if (!str || !env)
		return (0);
	eq = ft_strchr(str, '=');
	if (!eq)
	{
		env->value = ft_strdup("");
		if (!env->value)
			return (0);
		return (1);
	}
	val_len = ft_strlen(eq + 1);
	temp = ft_substr(str, (unsigned int)(eq - str + 1), val_len);
	if (!temp)
		return (0);
	env->value = ft_strdup(temp);
	free(temp);
	if (!env->value)
		return (0);
	return (1);
}

int push_env(t_env_list *lst, t_env_node *env)
{
	if (!lst || !env)
		return (0);
	env->next = NULL;
	if (!lst->head)
	{
		lst->head = env;
		lst->tail = env;
	}
	else
	{
		lst->tail->next = env;
		lst->tail = env;
	}
	lst->size++;
	return (1);
}

int get_envs(char **env, t_env_list *lst)
{
	int i;
	t_env_node *node;

	if (!env || !lst)
		return (0);
	i = 0;
	while (env[i])
	{
		node = malloc(sizeof(*node));
		if (!node)
			return (0);
		if (!find_key(env[i], node))
			return (0);
		if (!find_value(env[i], node))
			return (0);
		if (!push_env(lst, node))
			return (0);
		i++;
	}
	return (1);
}
