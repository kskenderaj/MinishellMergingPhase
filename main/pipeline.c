/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:03:19 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	handle_pipeline(t_cmd_list *cmdlst, t_env_list *env, t_shell_state *shell)
{
	char	**envp;
	int		ret;

	envp = env_list_to_array(env, shell);
	if (!envp)
		return (1);
	ret = exec_pipeline(cmdlst, envp, shell);
	return (ret);
}
