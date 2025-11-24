/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 17:51:02 by kskender          #+#    #+#             */
/*   Updated: 2025/11/24 14:28:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "garbage_collector.h"

char	*gc_strdup(t_gc *gc, const char *s)
{
	char	*dup;
	size_t	len;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = gc_malloc(gc, len + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, len);
	dup[len] = '\0';
	return (dup);
}

char	*gc_strdup_persistent(t_gc *gc, const char *s)
{
	char	*dup;
	size_t	len;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = gc_malloc_persistent(gc, len + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, len);
	dup[len] = '\0';
	return (dup);
}

char	*gc_strndup(t_gc *gc, const char *s, size_t n)
{
	char	*dup;
	size_t	length;

	if (!s)
		return (NULL);
	length = ft_strlen(s);
	if (length > n)
		length = n;
	dup = gc_malloc(gc, length + 1);
	if (!dup)
		return (NULL);
	ft_memcpy(dup, s, length);
	dup[length] = '\0';
	return (dup);
}

static size_t	count_words(const char *s, char c)
{
	size_t	count;
	int		in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static char	*extract_word(t_gc *gc, const char *s, char c)
{
	size_t	length;
	char	*word;

	length = 0;
	while (s[length] && s[length] != c)
		length++;
	word = gc_malloc(gc, length + 1);
	if (!word)
		return (NULL);
	ft_memcpy(word, s, length);
	word[length] = '\0';
	return (word);
}

char	**gc_split(t_gc *gc, const char *s, char c)
{
	char	**result;
	size_t	words;
	size_t	i;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	result = gc_calloc(gc, words + 1, sizeof(char *));
	if (!result)
		return (NULL);
	i = 0;
	while (*s)
	{
		if (*s != c)
		{
			result[i] = extract_word(gc, s, c);
			if (!result[i++])
				return (NULL);
			while (*s && *s != c)
				s++;
		}
		else
			s++;
	}
	return (result);
}
