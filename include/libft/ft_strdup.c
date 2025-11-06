/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:41:06 by kskender          #+#    #+#             */
/*   Updated: 2025/03/17 17:41:28 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *src)
{
	char	*str;
	int		buff_size;

	buff_size = (ft_strlen(src) + 1);
	str = malloc(buff_size);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, src, buff_size);
	return (str);
}
