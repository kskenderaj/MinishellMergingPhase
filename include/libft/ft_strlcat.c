/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 19:22:54 by kskender          #+#    #+#             */
/*   Updated: 2025/03/17 20:09:25 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t destsize)
{
	size_t	src_len;
	size_t	dest_len;
	size_t	empty_space;

	src_len = ft_strlen(src);
	if (dest == NULL && destsize == 0)
		return (src_len);
	dest_len = ft_strlen(dest);
	if (dest_len >= destsize)
		return (src_len + destsize);
	empty_space = destsize - dest_len - 1;
	if (empty_space > 0)
	{
		destsize -= dest_len;
		ft_strlcpy(dest + dest_len, src, empty_space + 1);
	}
	return (dest_len + src_len);
}
