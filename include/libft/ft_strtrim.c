/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 19:39:54 by kskender          #+#    #+#             */
/*   Updated: 2025/03/20 23:42:37 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(const char *str1, const char *bp)
{
	int		first;
	int		last;
	char	*trimmed;

	if (!str1 || !bp)
		return (NULL);
	if (!*str1)
		return (ft_strdup(str1));
	first = 0;
	last = ft_strlen(str1) - 1;
	while (*(str1 + first) && ft_strchr(bp, *(str1 + first)))
		++first;
	while (last >= 0 && ft_strchr(bp, *(str1 + last)))
		--last;
	if (first > last)
		return (ft_strdup(""));
	trimmed = malloc((last - first) + 2);
	if (trimmed == NULL)
		return (NULL);
	ft_strlcpy(trimmed, str1 + first, (last - first) + 2);
	return (trimmed);
}
