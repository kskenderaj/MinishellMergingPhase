/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ifs_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 19:56:39 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

/*
** ifs_field_split: Performs IFS field splitting on a string
** - Collapses consecutive whitespace to SINGLE space
** - PRESERVES leading/trailing spaces as field delimiters
** - Used only for unquoted variable expansions when adjacent to other segments
**
** Example: "  A  B  " -> " A B " (single spaces, but keeps boundary spaces)
*/
char *ifs_field_split(char *str)
{
    char *result;
    int i;
    int j;
    int prev_was_space;

    if (!str)
        return (NULL);
    result = gc_malloc(ft_strlen(str) + 1);
    if (!result)
        return (NULL);
    i = 0;
    j = 0;
    prev_was_space = 0;
    /* Process string, collapsing consecutive whitespace to single space */
    while (str[i])
    {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        {
            if (!prev_was_space)
            {
                result[j++] = ' ';
                prev_was_space = 1;
            }
            i++;
        }
        else
        {
            result[j++] = str[i++];
            prev_was_space = 0;
        }
    }
    result[j] = '\0';
    return (result);
}
