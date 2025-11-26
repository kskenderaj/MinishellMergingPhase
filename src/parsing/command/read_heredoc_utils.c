/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 16:17:52 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:18:49 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"

int is_delimiter(char *line, char *delimiter)
{
    size_t len;

    if (!line || !delimiter)
        return (0);
    len = ft_strlen(delimiter);
    if (len == 0)
        return (0);
    if (ft_strncmp(line, delimiter, len) == 0 && line[len] == '\0')
        return (1);
    return (0);
}