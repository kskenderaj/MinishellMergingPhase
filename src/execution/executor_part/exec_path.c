/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:57:24 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 15:37:46 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static char *build_path_candidate(char *dir, char *cmd, size_t len,
								  t_shell_state *shell)
{
	char *candidate;

	candidate = gc_malloc(shell->gc, len);
	if (!candidate)
		return (NULL);
	ft_strlcpy(candidate, dir, len);
	if (candidate[ft_strlen(candidate) - 1] != '/')
		ft_strlcat(candidate, "/", len);
	ft_strlcat(candidate, cmd, len);
	return (candidate);
}

static char *search_in_paths(char **paths, char *cmd, t_shell_state *shell)
{
	char *candidate;
	size_t len;
	int i;

	i = 0;
	while (paths[i])
	{
		len = ft_strlen(paths[i]) + 1 + ft_strlen(cmd) + 1;
		candidate = build_path_candidate(paths[i], cmd, len, shell);
		if (!candidate)
			break;
		if (access(candidate, X_OK) == 0)
			return (candidate);
		i++;
	}
	return (NULL);
}

char *find_in_path(char *cmd, t_shell_state *shell)
{
	char **paths;
	char *path_env;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (gc_strdup(shell->gc, cmd));
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = gc_split(shell->gc, path_env, ':');
	if (!paths)
		return (gc_strdup(shell->gc, cmd));
	return (search_in_paths(paths, cmd, shell));
}
