#include "minishell.h"
#include "garbage_collector.h"

int skip_var(char *str)
{
	int i;

	i = 1;
	if (!str || str[0] != '$')
		return (0);
	if (str[1] == '\0')
		return (1);
	if (str[1] == '?')
		return (2);
	if (!(ft_isalpha((unsigned char)str[1]) || str[1] == '_'))
		return (1);
	i = 2;
	while (str[i] && (ft_isalnum((unsigned char)str[i]) || str[i] == '_'))
		i++;
	return (i);
}

char *check_for_env(t_env_list *envlst, char *str, size_t len)
{
	t_env_node *node;

	if (!envlst || !str)
		return (NULL);
	node = envlst->head;
	while (node)
	{
		if (node->key && ft_strlen(node->key) == len && ft_strncmp(node->key, str, len) == 0)
			return (node->value);
		node = node->next;
	}
	return (NULL);
}

char *expand_env(char *str, t_env_list *env_lst)
{
	size_t i;
	size_t start;
	char *value;

	i = 1;
	if (!str || str[0] != '$' || !env_lst)
		return (NULL);
	if (!(ft_isalpha((unsigned char)str[i]) || str[i] == '_'))
		return (NULL);
	start = i;
	while (str[i] && (ft_isalnum((unsigned char)str[i]) || str[i] == '_'))
		i++;
	value = check_for_env(env_lst, str + start, i - start);
	if (value)
		return (gc_strdup(value));
	return (gc_strdup(""));
}

char *expand_or_not(char *seg_str, t_seg_type seg_type, t_env_list *envlst,
					int last_status, int i)
{
	char *old;
	char *expand;
	char *new;

	if (!seg_str)
		return (NULL);
	old = gc_strdup("");
	while (seg_str[i])
	{
		if (seg_str[i] == '$' && (seg_type == SEG_DOUBLE || seg_type == SEG_NO_QUOTE))
		{
			expand = get_expand(seg_str, i, last_status, envlst);
			new = gc_strjoin(old, expand);
			old = new;
			i += (skip_var(seg_str + i) - 1);
		}
		else
		{
			new = gc_strjoin(old, gc_substr(seg_str + i, 0, 1));
			old = new;
		}
		i++;
	}
	return (new);
}

char *segments_expand(t_segment_list *seglst, t_env_list *envlst,
					  int last_status)
{
	t_segment *curr;
	char *tmp;
	char *expansion;
	char *new_segment;

	if (!seglst)
		return (NULL);
	tmp = gc_substr("", 0, 0);
	curr = seglst->head;
	while (curr)
	{
		expansion = expand_or_not(curr->value, curr->type, envlst, last_status,
								  0);
		if (!expansion)
			return (NULL);
		new_segment = gc_strjoin(tmp, expansion);
		tmp = new_segment;
		curr = curr->next;
	}
	return (tmp);
}
