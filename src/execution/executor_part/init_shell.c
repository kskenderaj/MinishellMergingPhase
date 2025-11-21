/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 17:27:10 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/04 17:27:49 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void	init_shell(void)
{
	int i;

	g_shell.exported_count = 0;
	i = 0;
	while (i < MAX_EXPORTED)
	{
		g_shell.exported_vars[i] = NULL;
		i++;
	}
}