/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:42:49 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 15:42:49 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "minishell.h" // Adjust if t_cmd_node or t_env_list are in another header

int table_of_builtins(t_cmd_node *cmd, char **envp, int flag);
char **generate_env(t_env_list *env);

int ft_pwd(char **args);
int ft_echo(char **args);
int ft_cd(char **args);
int ft_export(char **args);
int ft_unset(char **args);
int ft_env(char **args);
int ft_exit(char **args);

#endif
