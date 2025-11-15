/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_code.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 02:22:03 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/14 20:12:09 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
int *exit_code(void)
{
    static int exit_status;
    return (&exit_status);
}

int is_exit_command(char *line)
{
    if (!line)
        return (0);
    if (ft_strcmp(line, "exit") == 0)
        return (1);
    return (0);
}