#include "parser.h"
#include "minishell.h"

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

char *get_expand(char *seg_str, int i, int last_status, t_env_list *envlst)
{
	if (seg_str[i + 1] == '?')
		return (gc_itoa(last_status));
	else
		return (expand_env(seg_str + i, envlst));
}

void remove_from_env_list(t_env_list *envlst, const char *key)
{
	t_env_node *prev;
	t_env_node *curr;

	if (!envlst || !key)
		return;
	prev = NULL;
	curr = envlst->head;
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else
				envlst->head = curr->next;
			if (curr == envlst->tail)
				envlst->tail = prev;
			if (curr->key)
				free(curr->key);
			if (curr->value)
				free(curr->value);
			free(curr);
			envlst->size--;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

void free_env_list(t_env_list *env)
{
	t_env_node *current;
	t_env_node *next;

	if (!env)
		return;
	current = env->head;
	while (current)
	{
		next = current->next;
		if (current->key)
			free(current->key);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
	if (env->pid)
		free(env->pid);
	free(env);
}
