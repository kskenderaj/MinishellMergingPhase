/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 17:27:10 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:27:21 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void	init_shell(t_shell_state *shell)
{
	int	i;

	shell->exported_count = 0;
	i = 0;
	while (i < MAX_EXPORTED)
	{
		shell->exported_vars[i] = NULL;
		i++;
	}
}
