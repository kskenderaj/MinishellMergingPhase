/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 15:59:56 by kskender          #+#    #+#             */
/*   Updated: 2025/03/19 16:02:45 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(const char *s, unsigned int start, size_t length)
{
	char	*str;
	size_t	string_length;
	size_t	substring_length;

	if (s == NULL)
		return (NULL);
	string_length = ft_strlen(s);
	if (start >= string_length)
		return (ft_strdup(""));
	substring_length = string_length - start;
	if (length > substring_length)
		length = substring_length;
	str = malloc(length + 1);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, s + start, length + 1);
	return (str);
}
