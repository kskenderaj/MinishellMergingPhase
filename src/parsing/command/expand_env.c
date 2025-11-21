/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/21 15:04:46 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"
#include "minishell.h"

static int	g_last_status_cache = 0;

int	skip_var(char *str)
{
	int	i;

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

char	*check_for_env(t_env_list *envlst, char *str, size_t len)
{
	t_env_node	*node;

	if (!envlst || !str)
		return (NULL);
	node = envlst->head;
	while (node)
	{
		if (node->key && ft_strlen(node->key) == len && ft_strncmp(node->key,
				str, len) == 0)
			return (node->value);
		node = node->next;
	}
	return (NULL);
}

char	*expand_env(char *str, t_env_list *env_lst)
{
	size_t	i;
	size_t	start;
	char	*value;

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

char	*process_dollar(char *seg_str, t_seg_type seg_type, t_env_list *envlst,
		int i)
{
	char	*expand;

	expand = get_expand(seg_str, i, g_last_status_cache, envlst);
	if (seg_type == SEG_NO && expand && ft_strchr(expand, ' '))
		expand = ifs_field_split(expand);
	return (expand);
}

char	*expand_or_not(char *seg_str, t_seg_type seg_type, t_env_list *envlst,
		int last_status)
{
	t_expand_ctx	ctx;
	char			*old;
	int				i;

	if (!seg_str)
		return (NULL);
	g_last_status_cache = last_status;
	ctx.seg_str = seg_str;
	ctx.type = seg_type;
	ctx.envlst = envlst;
	old = gc_strdup("");
	i = 0;
	while (seg_str[i])
	{
		process_char(&old, &ctx, &i);
		i++;
	}
	return (old);
}
