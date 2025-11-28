/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:18:20 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "garbage_collector.h"
#include "minishell.h"

int	skip_var(char *s)
{
	int	i;

	i = 1;
	if (s[1] == '?')
		return (2);
	if (!(ft_isalpha(s[1]) || s[1] == '_'))
		return (1);
	while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
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

char	*expand_env(char *str, t_env_list *env_lst, t_shell_state *shell)
{
	size_t	i;
	size_t	start;
	char	*value;

	i = 1;
	if (!str || str[0] != '$' || !env_lst)
		return (NULL);
	if (!str[i] || !(ft_isalpha(str[i]) || str[i] == '_' || ft_isdigit(str[i])))
		return (gc_strdup(shell->gc, "$"));
	start = i;
	while (str[i] && (ft_isalnum((unsigned char)str[i]) || str[i] == '_'))
		i++;
	value = check_for_env(env_lst, str + start, i - start);
	if (value)
		return (gc_strdup(shell->gc, value));
	return (gc_strdup(shell->gc, ""));
}

char	*process_dollar(t_expand_ctx *ctx, int i, t_shell_state *shell)
{
	static int	g_last_status_cache;
	char		*expand;

	g_last_status_cache = 0;
	expand = get_expand(ctx->seg_str, i, ctx->envlst, shell);
	if (ctx->type == SEG_NO && expand && ft_strchr(expand, ' '))
		expand = ifs_field_split(expand, shell);
	return (expand);
}

char	*expand_or_not(t_expand_ctx *ctx, int last_status, t_shell_state *shell)
{
	char	*old;
	int		i;

	(void)last_status;
	if (!ctx->seg_str)
		return (NULL);
	old = gc_strdup(shell->gc, "");
	i = 0;
	while (ctx->seg_str[i])
	{
		process_char(&old, ctx, &i, shell);
		i++;
	}
	return (old);
}
