/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:57:24 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/04 17:07:34 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

char *find_in_path(char *cmd)
{
	char **paths;
	char *path_env;
	char *candidate;
	size_t len;
	int i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (gc_strdup(cmd));
	path_env = getenv("PATH");
	if (!path_env)
		return (gc_strdup(cmd));
	paths = gc_split(path_env, ':');
	if (!paths)
		return (gc_strdup(cmd));
	i = 0;
	while (paths[i])
	{
		len = ft_strlen(paths[i]) + 1 + ft_strlen(cmd) + 1;
		candidate = gc_malloc(len);
		if (!candidate)
			break;
		ft_strlcpy(candidate, paths[i], len);
		if (candidate[ft_strlen(candidate) - 1] != '/')
			ft_strlcat(candidate, "/", len);
		ft_strlcat(candidate, cmd, len);
		if (access(candidate, X_OK) == 0)
			return (candidate);
		i++;
	}
	return (gc_strdup(cmd));
}
