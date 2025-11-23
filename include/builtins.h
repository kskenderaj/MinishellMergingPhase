/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 15:57:19 by kskender          #+#    #+#             */
/*   Updated: 2025/11/23 15:58:05 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "minishell.h"

#ifndef BUILTINS_H
#define BUILTINS_H


typedef struct s_shell_state	t_shell_state;

int table_of_builtins(t_cmd_node *cmd, char **envp, int flag, t_shell_state *shell);
char **generate_env(t_env_list *env, t_shell_state *shell);

int ft_pwd(char **args, t_shell_state *shell);
int ft_echo(char **args, t_shell_state *shell);
int ft_cd(char **args, t_shell_state *shell);
int ft_export(char **args, t_shell_state *shell);
int ft_unset(char **args, t_shell_state *shell);
int ft_env(char **args, t_shell_state *shell);

#endif
