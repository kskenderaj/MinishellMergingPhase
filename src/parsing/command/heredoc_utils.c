/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 14:16:43 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

static int has_quotes(char *str)
{
    int i;

    i = 0;
    while (str[i])
    {
        if (str[i] == '\'' || str[i] == '"')
            return (1);
        i++;
    }
    return (0);
}

static char *remove_quotes_heredoc(char *str)
{
    char *result;
    int i;
    int j;
    char quote;

    result = gc_malloc(ft_strlen(str) + 1);
    if (!result)
        return (NULL);
    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] == '\'' || str[i] == '"')
        {
            quote = str[i++];
            while (str[i] && str[i] != quote)
                result[j++] = str[i++];
            if (str[i] == quote)
                i++;
        }
        else
            result[j++] = str[i++];
    }
    result[j] = '\0';
    return (result);
}

t_heredoc_info *process_heredoc_delimiter(char *raw_delimiter)
{
    t_heredoc_info *info;

    if (!raw_delimiter)
        return (NULL);
    info = gc_malloc(sizeof(t_heredoc_info));
    if (!info)
        return (NULL);
    info->quoted = has_quotes(raw_delimiter);
    if (info->quoted)
        info->delimiter = remove_quotes_heredoc(raw_delimiter);
    else
        info->delimiter = gc_strdup(raw_delimiter);
    if (!info->delimiter)
        return (NULL);
    return (info);
}
