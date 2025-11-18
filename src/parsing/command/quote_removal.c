/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_removal.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/18 01:00:00 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "garbage_collector.h"

// static char	*remove_quotes_from_string(char *str)
// {
// 	char	*result;
// 	int		i;
// 	int		j;
// 	char	quote;

// 	if (!str)
// 		return (NULL);
// 	result = gc_malloc(ft_strlen(str) + 1);
// 	if (!result)
// 		return (str);
// 	i = 0;
// 	j = 0;
// 	quote = 0;
// 	while (str[i])
// 	{
// 		if ((str[i] == '\'' || str[i] == '"') && !quote)
// 		{
// 			quote = str[i];
// 			i++;
// 			continue ;
// 		}
// 		if (str[i] == quote)
// 		{
// 			quote = 0;
// 			i++;
// 			continue ;
// 		}
// 		result[j++] = str[i++];
// 	}
// 	result[j] = '\0';
// 	return (result);
// }

// char	*remove_quotes(char *str)
// {
// 	// QUOTES ARE ALREADY REMOVED BY SEGMENT PARSING!
// 	// Segments are created WITHOUT the enclosing quotes.
// 	// So we just return the string as-is.
// 	return (str);
// }
