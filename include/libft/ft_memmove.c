/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 14:04:42 by kskender          #+#    #+#             */
/*   Updated: 2025/03/18 17:58:15 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t length)
{
	char		*copy_dest;
	const char	*copy_src;
	size_t		i;

	copy_dest = (char *) dest;
	copy_src = (char *) src;
	if (!dest && !src)
		return (NULL);
	if (copy_dest > copy_src)
	{
		while (length-- > 0)
			copy_dest[length] = copy_src[length];
	}
	else
	{
		i = 0;
		while (i < length)
		{
			copy_dest[i] = copy_src[i];
			i++;
		}
	}
	return (dest);
}
