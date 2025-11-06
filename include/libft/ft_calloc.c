/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:02:57 by kskender          #+#    #+#             */
/*   Updated: 2025/03/18 16:32:37 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmele, size_t size)
{
	size_t	bytes;
	size_t	i;
	void	*ptr;

	bytes = nmele * size;
	if (size && (bytes / size != nmele))
		return (NULL);
	ptr = malloc(bytes);
	if (ptr == NULL)
		return (NULL);
	i = 0;
	while (i < bytes)
	{
		((unsigned char *)ptr)[i] = 0;
		i++;
	}
	return (ptr);
}
