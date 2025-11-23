/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector_parser.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 15:59:49 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 15:55:09 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

char	*gc_substr(t_gc *gc, const char *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	slen;

	if (!s)
		return (NULL);
	slen = ft_strlen(s);
	if (start >= slen)
		return (gc_strdup(gc, ""));
	if (len > slen - start)
		len = slen - start;
	sub = gc_malloc(gc, len + 1);
	if (!sub)
		return (NULL);
	ft_memcpy(sub, s + start, len);
	sub[len] = '\0';
	return (sub);
}

char	*gc_itoa(t_gc *gc, int n)
{
	char	*tmp;
	char	*res;

	tmp = ft_itoa(n);
	if (!tmp)
		return (NULL);
	res = gc_strdup(gc, tmp);
	free(tmp);
	return (res);
}

char	*gc_strjoin(t_gc *gc, const char *s1, const char *s2)
{
	char	*tmp;
	char	*res;

	if (!s1 && !s2)
		return (gc_strdup(gc, ""));
	if (!s1)
		return (gc_strdup(gc, s2));
	if (!s2)
		return (gc_strdup(gc, s1));
	tmp = ft_strjoin(s1, s2);
	if (!tmp)
		return (NULL);
	res = gc_strdup(gc, tmp);
	free(tmp);
	return (res);
}

size_t	gc_count(t_gc *gc)
{
	if (!gc)
		return (0);
	return (gc->count);
}