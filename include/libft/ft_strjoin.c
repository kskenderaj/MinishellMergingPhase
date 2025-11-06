/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 17:37:35 by kskender          #+#    #+#             */
/*   Updated: 2025/03/20 19:27:41 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *str1, const char *str2)
{
	size_t	buffer;
	size_t	i;
	char	*ptr;

	i = 0;
	if (str1 == NULL || str2 == NULL)
		return (NULL);
	buffer = ft_strlen(str1) + ft_strlen(str2) + 1;
	ptr = malloc(buffer);
	if (ptr == NULL)
		return (NULL);
	while (*str1)
		*(ptr + i++) = *str1++;
	while (*str2)
		*(ptr + i++) = *str2++;
	*(ptr + i) = '\0';
	return (ptr);
}
