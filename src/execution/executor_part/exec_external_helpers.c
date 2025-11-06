/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 16:26:38 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/03 15:44:20 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

// Searches $PATH for executable
char	*find_command_path(char *cmd, t_env_list *env)
{
	char	*path_env;
	char	**dirs;
	char	full_path[1024];
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	dirs = ft_split(path_env, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		ft_strlcpy(full_path, dirs[i], sizeof(full_path));
		ft_strlcat(full_path, "/", sizeof(full_path));
		ft_strlcat(full_path, cmd, sizeof(full_path));
		if (access(full_path, X_OK) == 0)
		{
			ft_free_array(dirs);
			return (ft_strdup(full_path));
		}
		i++;
	}
	ft_free_array(dirs);
	return (NULL);
}

char	**convert_env_to_array(t_env_list *env_list)
{
	extern char	**environ;
	char		**env_array;
	int			i;

	(void)env_list;
	if (!environ)
		return (NULL);
	i = 0;
	while (environ[i])
		i++;
	env_array = gc_malloc(sizeof(char *) * (i + 1));
	if (!env_array)
		return (NULL);
	i = 0;
	while (environ[i])
	{
		env_array[i] = ft_strdup(environ[i]);
		i++;
	}
	env_array[i] = NULL;
	return (env_array);
}
