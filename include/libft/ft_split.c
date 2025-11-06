/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 22:45:23 by kskender          #+#    #+#             */
/*   Updated: 2025/03/20 17:27:02 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_words(const char *str, char delimiter)
{
	size_t	count;
	int		inword;

	count = 0;
	inword = 0;
	while (*str)
	{
		if (*str != delimiter && !inword)
		{
			inword = 1;
			count++;
		}
		else if (*str == delimiter)
			inword = 0;
		str++;
	}
	return (count);
}

static char	*copyword(const char *start, size_t length)
{
	char	*word;
	size_t	i;

	word = (char *)malloc(length + 1);
	if (!word)
		return (NULL);
	i = 0;
	while (i < length)
	{
		word[i] = start[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static size_t	ft_wordlen(const char *str, char delim)
{
	size_t	length;

	length = 0;
	while (str[length] && str[length] != delim)
		length++;
	return (length);
}

static void	free_split(char **result, size_t i)
{
	while (i > 0)
		free(result[--i]);
	free(result);
}

char	**ft_split(char const *str, char delim)
{
	char	**result;
	size_t	length;
	size_t	i;

	if (!str)
		return (NULL);
	result = (char **)malloc((count_words(str, delim) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	i = 0;
	while (*str)
	{
		if (*str != delim)
		{
			length = ft_wordlen(str, delim);
			result[i] = copyword(str, length);
			if (!result[i])
				return (free_split(result, i), NULL);
			i++;
			str += length;
		}
		else
			str++;
	}
	return (result[i] = NULL, result);
}
