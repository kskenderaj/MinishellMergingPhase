/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:58:58 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/06 18:14:09 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* helpers for assignment and expansion */
static int	is_assignment(const char *s)
{
	const char	*p;

	if (!s || !*s)
		return (0);
	p = s;
	if (!(ft_isalpha((unsigned char)*p) || *p == '_'))
		return (0);
	p++;
	while (*p && *p != '=')
	{
		if (!(ft_isalnum((unsigned char)*p) || *p == '_'))
			return (0);
		p++;
	}
	return (*p == '=');
}

/* build an envp array merging current environ and local assignments (KEY=VAL strings)
 * returned array is gc_malloc'd and safe to pass to execve
 */
static char	**build_envp_from_local(char **local)
{
	extern char	**environ;
	int			envc;
	int			expc;
	int			localc;
	char		**out;
	int			outi;
	int			found;
	int			elen;
	char		*entry;
	char		*eq;
	int			namelen;
	int			replaced;
	const char	*ename;
	int			j;

	/* 'ename' will be fetched inside the loop below */
	/* will fetch exported names inside the loop below */
	/* Build envp by taking environ,
		adding exported-only names that are not in environ,
		* then appending local assignments (which override existing names). Avoid duplicates.
		*/
	envc = 0;
	while (environ && environ[envc])
		envc++;
	expc = g_shell.exported_count;
	localc = 0;
	while (local && local[localc])
		localc++;
	/* allocate worst-case size: environ + exported + local + NULL */
	out = gc_malloc(sizeof(char *) * (envc + expc + localc + 1));
	outi = 0;
	/* copy environ first */
	for (int i = 0; i < envc; ++i)
		out[outi++] = gc_strdup(environ[i]);
	/* add exported names that are not present in environ as NAME= (empty value) */
	for (j = 0; j < expc; ++j)
	{
		ename = g_shell.exported_vars[j];
		if (!ename)
			continue ;
		found = 0;
		elen = ft_strlen(ename);
		for (int k = 0; k < outi; ++k)
		{
			if (ft_strncmp(out[k], ename, elen) == 0 && out[k][elen] == '=')
			{
				found = 1;
				break ;
			}
		}
		if (!found)
		{
			entry = gc_malloc((size_t)elen + 2);
			if (entry)
			{
				memcpy(entry, ename, (size_t)elen);
				entry[elen] = '=';
				entry[elen + 1] = '\0';
				out[outi++] = entry;
			}
		}
	}
	/* now apply local assignments, overriding any existing name in out */
	for (int l = 0; l < localc; ++l)
	{
		eq = ft_strchr(local[l], '=');
		if (!eq)
			continue ;
		namelen = (int)(eq - local[l]);
		replaced = 0;
		for (int m = 0; m < outi; ++m)
		{
			if (ft_strncmp(out[m], local[l], (size_t)namelen) == 0
				&& out[m][namelen] == '=')
			{
				out[m] = gc_strdup(local[l]);
				replaced = 1;
				break ;
			}
		}
		if (!replaced)
			out[outi++] = gc_strdup(local[l]);
	}
	out[outi] = NULL;
	return (out);
}

/* apply assignments temporarily in the current process (for builtins). prev_values

	* is a gc_malloc'd array sized to local_count*2: pairs of key and previous value (or NULL)
 */
static char	**apply_assignments_temp(char **local)
{
	int		count;
	char	**prev;
	int		i;
	char	*eq;
	char	*key;
	char	*val;

	if (!local)
		return (NULL);
	count = 0;
	while (local[count])
		count++;
	prev = gc_malloc(sizeof(char *) * (count * 2 + 1));
	i = 0;
	for (int k = 0; k < count; ++k)
	{
		eq = ft_strchr(local[k], '=');
		if (!eq)
			continue ;
		key = gc_substr(local[k], 0, (unsigned int)(eq - local[k]));
		val = eq + 1;
		prev[i++] = key;
		prev[i++] = getenv(key) ? gc_strdup(getenv(key)) : NULL;
		setenv(key, val, 1);
	}
	prev[i] = NULL;
	return (prev);
}

static void	restore_assignments(char **prev)
{
	int		i;
	char	*key;
	char	*old;

	if (!prev)
		return ;
	i = 0;
	while (prev[i])
	{
		key = prev[i++];
		old = prev[i++];
		if (old)
			setenv(key, old, 1);
		else
			unsetenv(key);
	}
}

static void	expand_args_inplace(char **args)
{
	int			i;
	char		*p;
	int			allow_expand;
	char		*stripped;
	char		*dollar;
	char		*start;
	int			len;
	char		*varname;
	char		*prefix;
	char		*suffix;
	char		*joined;
	char		*src;
	int			j;
	char		*clean;
	const char	*name = NULL;
	const char	*v = NULL;

	i = 0;
	while (args && args[i])
	{
		p = args[i];
		allow_expand = 1;
		if (p && (unsigned char)p[0] == 0x01)
		{
			/* single-quoted: strip marker and do not expand */
			stripped = gc_strdup(p + 1);
			args[i] = stripped;
			p = args[i];
			allow_expand = 0;
		}
		else if (p && (unsigned char)p[0] == 0x02)
		{
			/* double-quoted: strip marker and allow expansions below */
			stripped = gc_strdup(p + 1);
			args[i] = stripped;
			p = args[i];
			allow_expand = 1;
		}
		if (allow_expand && p && *p == '$')
		{
			/* simple $VAR: only expand if NAME starts with letter or underscore */
			name = p + 1;
			v = getenv(name);
			if (name && (*name == '_' || ft_isalpha((unsigned char)*name)))
			{
				if (v)
					args[i] = gc_strdup(v);
				else
					args[i] = gc_strdup("");
			}
			else
			{
				/* leave as-is (do not strip positional parameters like $2) */
				args[i] = gc_strdup(p);
			}
		}
		else
		{
			/* support $VAR inside string (simple) */
			if (allow_expand && p)
			{
				dollar = ft_strchr(p, '$');
				if (dollar)
				{
					/* only handle single $VAR occurrence where NAME starts with letter or underscore */
					start = p;
					name = dollar + 1;
					if (name && (*name == '_'
							|| ft_isalpha((unsigned char)*name)))
					{
						len = 0;
						while (name[len]
							&& (ft_isalnum((unsigned char)name[len])
								|| name[len] == '_'))
							len++;
						if (len > 0)
						{
							varname = gc_substr(name, 0, (unsigned int)len);
							/* lookup value for this variable name */
							v = getenv(varname);
							prefix = gc_substr(start, 0, (unsigned int)(dollar
										- start));
							suffix = gc_strdup(name + len);
							joined = NULL;
							if (v)
							{
								joined = gc_strjoin(prefix, v);
								joined = gc_strjoin(joined, suffix);
							}
							else
							{
								joined = gc_strjoin(prefix, suffix);
							}
							args[i] = joined;
						}
					}
				}
			}
		}
		if (p && !allow_expand)
		{
			src = args[i];
			j = 0;
			clean = gc_malloc(ft_strlen(src) + 1);
			if (clean)
			{
				for (int k = 0; src[k]; ++k)
				{
					if (src[k] == '\\' || src[k] == '\'' || src[k] == '"')
						continue ;
					clean[j++] = src[k];
				}
				clean[j] = '\0';
				args[i] = clean;
			}
		}
		i++;
	}
}

/* Forward declarations from other modules */

/* collect_assignment: return a GC-allocated copy of an assignment token
 * kept as a small helper so callers can rely on ownership semantics.
 */
static char	*collect_assignment(char *s)
{
	if (!s)
		return (NULL);
	return (gc_strdup(s));
}

void		exec_external(char **args, char **envp);
void		split_args(char *input, char **args, int max_args);

static int	is_builtin(const char **args)
{
	if (!args || !args[0])
		return (0);
	if (!strcmp(args[0], "echo"))
		return (1);
	if (!strcmp(args[0], "cd"))
		return (1);
	if (!strcmp(args[0], "pwd"))
		return (1);
	if (!strcmp(args[0], "export"))
		return (1);
	if (!strcmp(args[0], "unset"))
		return (1);
	if (!strcmp(args[0], "env"))
		return (1);
	return (0);
}

static int	run_external(char **args, int in_fd, int out_fd, char **envp)
{
	pid_t		pid;
	int			status;
	extern char	**environ;

	pid = fork();
	if (pid == 0)
	{
		if (in_fd != -1)
		{
			dup2(in_fd, STDIN_FILENO);
			close(in_fd);
		}
		if (out_fd != -1)
		{
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);
		}
		exec_external(args, envp ? envp : environ);
		/* Per strict test expectations,
			print only the message 'command not found' */
		ft_putstr_fd("command not found\n", STDERR_FILENO);
		_exit(127);
	}
	if (in_fd != -1)
		close(in_fd);
	if (out_fd != -1)
		close(out_fd);
	if (pid > 0)
		waitpid(pid, &status, 0);
	else
		perror("fork");
	return (0);
}

static int	build_and_run_pipeline(char **args, char **envp)
{
	char		*tmp_argv[32];
	char		**cmd_argvs[32];
	char		**argv_gc;
	int			ci;
	int			ai;
	int			ti;
	int			x;
	extern char	**environ;

	ci = 0;
	ai = 0;
	while (args[ai])
	{
		ti = 0;
		while (args[ai] && strcmp(args[ai], "|") != 0)
			tmp_argv[ti++] = args[ai++];
		tmp_argv[ti] = NULL;
		argv_gc = gc_malloc(sizeof(char *) * (ti + 1));
		x = 0;
		while (x <= ti)
		{
			argv_gc[x] = tmp_argv[x];
			x++;
		}
		cmd_argvs[ci++] = argv_gc;
		if (args[ai] && !strcmp(args[ai], "|"))
			ai++;
	}
	/* expand args in each pipeline command (handle quoted markers and $VAR) */
	for (int z = 0; z < ci; ++z)
		expand_args_inplace(cmd_argvs[z]);
	/* parent debug removed */
	exec_pipeline(cmd_argvs, ci, envp ? envp : environ);
	return (0);
}

static int	execute_builtin_with_possible_redir(char **args, int in_fd,
		int out_fd)
{
	if (in_fd != -1 || out_fd != -1)
	{
		if (!strcmp(args[0], "echo"))
			exec_builtin_with_redir(ft_echo, args, in_fd, out_fd);
		else if (!strcmp(args[0], "cd"))
			exec_builtin_with_redir(ft_cd, args, in_fd, out_fd);
		else if (!strcmp(args[0], "pwd"))
			exec_builtin_with_redir(ft_pwd, args, in_fd, out_fd);
		else if (!strcmp(args[0], "export"))
			exec_builtin_with_redir(ft_export, args, in_fd, out_fd);
		else if (!strcmp(args[0], "unset"))
			exec_builtin_with_redir(ft_unset, args, in_fd, out_fd);
		else if (!strcmp(args[0], "env"))
			exec_builtin_with_redir(ft_env, args, in_fd, out_fd);
	}
	else
	{
		if (!strcmp(args[0], "echo"))
			ft_echo(args);
		else if (!strcmp(args[0], "cd"))
			ft_cd(args);
		else if (!strcmp(args[0], "pwd"))
			ft_pwd(args);
		else if (!strcmp(args[0], "export"))
			ft_export(args);
		else if (!strcmp(args[0], "unset"))
			ft_unset(args);
		else if (!strcmp(args[0], "env"))
			ft_env(args);
	}
	return (0);
}

int	main(void)
{
	char input[1024];
	char *args[32];
	char *p;
	int in_fd;
	int out_fd;
	int i;
	int handled;

	char *local_assigns[32];
	int la_i;
	int read;
	int write;
	int is_export_cmd;
	char *eq;
	char *key;
	char *val;
	char **cmd_envp;
	char **prev;

	/* initialize shell globals and signals */
	init_shell();
	start_signals();
	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("minishell$ ");
		if (!fgets(input, sizeof(input), stdin))
			break ;
		input[strcspn(input, "\n\r")] = 0;
		/* skip comment-only lines */
		p = input;
		while (*p && isspace((unsigned char)*p))
			p++;
		if (*p == '#')
			continue ;
		if (!strcmp(input, "exit"))
			break ;
		split_args(input, args, 32);
		if (!args[0])
			continue ;
		/* collect assignments anywhere in args into local_assigns[] and remove them
			* but if the command is `export` we should keep assignments as arguments
			* so ft_export can handle 'export VAR=val' semantics.
			*/
		la_i = 0;
		read = 0;
		write = 0;
		is_export_cmd = (args[0] && !strcmp(args[0], "export"));
		if (is_export_cmd)
		{
			/* do not strip assignments; leave args intact */
			local_assigns[0] = NULL;
		}
		else
		{
			/* debug logging removed */
			while (args[read])
			{
				if (is_assignment(args[read]))
				{
					local_assigns[la_i++] = collect_assignment(args[read]);
					read++;
					continue ;
				}
				args[write++] = args[read++];
			}
			args[write] = NULL;
			local_assigns[la_i] = NULL;
			/* debug logging removed */
		}
		if (!args[0])
		{
			/* no command: assignments become shell variables */
			for (int k = 0; k < la_i; ++k)
			{
				eq = ft_strchr(local_assigns[k], '=');
				if (!eq)
					continue ;
				key = gc_substr(local_assigns[k], 0, (unsigned int)(eq
							- local_assigns[k]));
				val = eq + 1;
				if (key)
					setenv(key, val ? val : "", 1);
			}
			continue ;
		}
		in_fd = -1;
		out_fd = -1;
		if (setup_redirections(args, &in_fd, &out_fd))
			continue ;
		i = 0;
		handled = 0;
		/* build per-command envp if local assignments exist */
		cmd_envp = NULL;
		if (local_assigns[0])
			cmd_envp = build_envp_from_local(local_assigns);
		/* debug logging removed */
		while (args[i])
		{
			if (!strcmp(args[i], "|"))
			{
				build_and_run_pipeline(args, cmd_envp);
				handled = 1;
				break ;
			}
			i++;
		}
		if (handled)
		{
			continue ;
		}
		if (is_builtin((const char **)args))
		{
			/* for builtins: apply assignments temporarily, run,
				then restore */
			prev = NULL;
			if (local_assigns[0])
				prev = apply_assignments_temp(local_assigns);
			expand_args_inplace(args);
			execute_builtin_with_possible_redir(args, in_fd, out_fd);
			if (prev)
				restore_assignments(prev);
			continue ;
		}
		expand_args_inplace(args);
		run_external(args, in_fd, out_fd, cmd_envp);
	}
	return (0);
}