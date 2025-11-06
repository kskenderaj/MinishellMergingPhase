/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:31:03 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/04 15:31:41 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
void print_command_error(char *cmd, char *error_msg)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(cmd, STDERR_FILENO);
    ft_putstr_fd(": ", STDERR_FILENO);
    ft_putstr_fd(error_msg, STDERR_FILENO);
    ft_putchar_fd('\n', STDERR_FILENO);
}

void print_file_error(char *filename, char *error_msg)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(filename, STDERR_FILENO);
    ft_putstr_fd(": ", STDERR_FILENO);
    ft_putstr_fd(error_msg, STDERR_FILENO);
    ft_putchar_fd('\n', STDERR_FILENO);
}

int handle_exec_error(char *cmd_path, char **args)
{
    if (access(cmd_path, F_OK) != 0)
    {
        print_command_error(args[0], "command not found");
        return (127);
    }
    if (access(cmd_path, X_OK) != 0)
    {
        print_command_error(args[0], "Permission denied");
        return (126);
    }
    return (1);
}