/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_basics1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:08:24 by kskender          #+#    #+#             */
/*   Updated: 2025/10/23 17:50:43 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void close_and_reset(int *prev_in_out, int *new_in_out, int *reset)
{
	if (prev_in_out[0] != NO_REDIRECTION)
	{
		gc_close(prev_in_out[0]);
		prev_in_out[0] = NO_REDIRECTION;
	}
	if (new_in_out[1] != NO_REDIRECTION)
	{
		gc_close(new_in_out[1]);
		new_in_out[1] = NO_REDIRECTION;
	}
	reset[0] = gc_dup(STDIN_FILENO);
	reset[1] = gc_dup(STDOUT_FILENO);
}
