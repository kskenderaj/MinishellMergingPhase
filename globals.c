/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 20:04:05 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 20:04:07 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* globals.c — single definition of g_shell */

#include "executor.h"

/* Ensure default empty exported lists */

t_shell_state g_shell = {{NULL}, 0, 0, NULL, 0};
