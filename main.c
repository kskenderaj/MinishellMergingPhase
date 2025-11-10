/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*        		t_token_list tokens;
		init_token_lst(&tokens);
		if (tokenize(&tokens, input) != 0)
		{
			g_shell.last_status = 2;
			free(input);
			continue;
		}
		t_cmd_list cmds;
		init_cmd_lst(&cmds);                              +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 00:58:58 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/07 20:27:17 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "main_helpers.h"
#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <glob.h>

/* helpers for assignment and expansion (is_assignment and collect_assignment
	were moved to src/main/assigns.c so they can be used by multiple helpers) */

/* build an envp array merging current environ and local assignments (KEY=VAL strings)
 * returned array is gc_malloc'd and safe to pass to execve
 */
static char **build_envp_from_local(char **local)
{
	extern char **environ;
	int envc;
	int expc;
	int localc;
	char **out;
	int outi;
	int found;
	int elen;
	char *entry;
	char *eq;
	int namelen;
	int replaced;
	const char *ename;
	int j;

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
			continue;
		found = 0;
		elen = ft_strlen(ename);
		for (int k = 0; k < outi; ++k)
		{
			if (ft_strncmp(out[k], ename, elen) == 0 && out[k][elen] == '=')
			{
				found = 1;
				break;
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
			continue;
		namelen = (int)(eq - local[l]);
		replaced = 0;
		for (int m = 0; m < outi; ++m)
		{
			if (ft_strncmp(out[m], local[l], (size_t)namelen) == 0 && out[m][namelen] == '=')
			{
				out[m] = gc_strdup(local[l]);
				replaced = 1;
				break;
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
char **apply_assignments_temp(char **local)
{
	int count;
	char **prev;
	int i;
	char *eq;
	char *key;
	char *val;

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
			continue;
		key = gc_substr(local[k], 0, (unsigned int)(eq - local[k]));
		val = eq + 1;
		prev[i++] = key;
		prev[i++] = getenv(key) ? gc_strdup(getenv(key)) : NULL;
		setenv(key, val, 1);
	}
	prev[i] = NULL;
	return (prev);
}

void restore_assignments(char **prev)
{
	int i;
	char *key;
	char *old;

	if (!prev)
		return;
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

void expand_args_inplace(char **args)
{
	int i;
	char *p;
	int allow_expand;
	char *stripped;
	char *dollar;
	char *start;
	int len;
	char *varname;
	char *prefix;
	char *suffix;
	char *joined;
	const char *name = NULL;
	const char *v = NULL;
	char was_quoted[32] = {0};

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
			was_quoted[i] = 1;
		}
		else if (p && (unsigned char)p[0] == 0x02)
		{
			/* double-quoted: strip marker and allow expansions below */
			stripped = gc_strdup(p + 1);
			args[i] = stripped;
			p = args[i];
			allow_expand = 1;
			was_quoted[i] = 2;
		}
		if (allow_expand && p && *p == '$')
		{
			/* simple $VAR: only expand if NAME starts with letter or underscore */
			name = p + 1;
			/* handle special $? expansion */
			if (name && name[0] == '?')
			{
				char *code = gc_itoa(g_shell.last_status);
				args[i] = code ? code : gc_strdup("");
			}
			else
			{
				v = check_for_env(g_shell.env, name, ft_strlen(name));
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
		}
		else
		{
			/* support $VAR inside string (recursive) */
			if (allow_expand && p)
			{
				while ((dollar = ft_strchr(p, '$')))
				{
					/* only handle single $VAR occurrence where NAME starts with letter or underscore */
					start = p;
					name = dollar + 1;
					/* support $? inside string */
					if (name && name[0] == '?')
					{
						char *num = gc_itoa(g_shell.last_status);
						prefix = gc_substr(start, 0, (unsigned int)(dollar - start));
						suffix = gc_strdup(name + 1);
						joined = NULL;
						if (num)
						{
							joined = gc_strjoin(prefix, num);
							joined = gc_strjoin(joined, suffix);
							gc_free(num);
						}
						else
							joined = gc_strjoin(prefix, suffix);
						args[i] = joined;
						p = args[i];
					}
					else if (name && (*name == '_' || ft_isalpha((unsigned char)*name)))
					{
						len = 0;
						while (name[len] && (ft_isalnum((unsigned char)name[len]) || name[len] == '_'))
							len++;
						if (len > 0)
						{
							varname = gc_substr(name, 0, (unsigned int)len);
							/* lookup value for this variable name */
							v = check_for_env(g_shell.env, varname, ft_strlen(varname));
							prefix = gc_substr(start, 0, (unsigned int)(dollar - start));
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
							p = args[i];
						}
						else
						{
							/* no valid var, skip this $ */
							p = dollar + 1;
						}
					}
					else
					{
						/* not valid, skip */
						p = dollar + 1;
					}
				}
			}
		}
		i++;
	}
	/* wildcard expansion */
	i = 0;
	while (args && args[i])
	{
		if (was_quoted[i])
		{
			i++;
			continue;
		}
		if (ft_strchr(args[i], '*'))
		{
			glob_t globbuf;
			if (glob(args[i], GLOB_NOCHECK, NULL, &globbuf) == 0)
			{
				if (globbuf.gl_pathc == 1)
				{
					args[i] = gc_strdup(globbuf.gl_pathv[0]);
				}
				else if (globbuf.gl_pathc > 1)
				{
					// find the end
					int j = i;
					while (args[j])
						j++;
					int total_args = j;
					int remaining = total_args - i - 1;
					if (i + globbuf.gl_pathc + remaining < 32)
					{
						// shift
						for (int k = total_args; k > i; k--)
						{
							args[k + globbuf.gl_pathc - 1] = args[k];
						}
						// insert
						for (size_t k = 0; k < globbuf.gl_pathc; k++)
						{
							args[i + k] = gc_strdup(globbuf.gl_pathv[k]);
						}
						i += globbuf.gl_pathc - 1;
					}
					else
					{
						// join
						char *joined = gc_strdup(globbuf.gl_pathv[0]);
						for (size_t k = 1; k < globbuf.gl_pathc; k++)
						{
							char *temp = gc_strjoin(joined, " ");
							joined = gc_strjoin(temp, globbuf.gl_pathv[k]);
						}
						args[i] = joined;
					}
				}
				globfree(&globbuf);
			}
		}
		i++;
	}
}

/* Forward declarations from other modules */

/* collect_assignment moved to src/main/assigns.c */

void exec_external(char **args, char **envp);
void split_args(char *input, char **args, int max_args);

int has_unmatched_quotes(char *str)
{
	int i = 0;
	int in_single = 0;
	int in_double = 0;

	while (str[i])
	{
		if (str[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (str[i] == '"' && !in_single)
			in_double = !in_double;
		i++;
	}
	return in_single || in_double;
}

int is_builtin(const char **args)
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

int run_external(char **args, int in_fd, int out_fd, char **envp)
{
	pid_t pid;
	int status;
	extern char **environ;

	pid = fork();
	if (pid == 0)
	{
		if (in_fd == -2)
		{
			close(STDIN_FILENO);
			in_fd = -1;
		}
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
	{
		if (waitpid(pid, &status, 0) > 0)
		{
			if (WIFEXITED(status))
				g_shell.last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				g_shell.last_status = 128 + WTERMSIG(status);
			else
				g_shell.last_status = 128;
		}
		else
			perror("waitpid");
	}
	else
		perror("fork");
	return (g_shell.last_status);
}

int build_and_run_pipeline(char **args, char **envp)
{
	char *tmp_argv[32];
	char **cmd_argvs[32];
	char **argv_gc;
	int ci;
	int ai;
	int ti;
	int x;
	extern char **environ;

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

int execute_builtin_with_possible_redir(char **args, int in_fd,
										int out_fd)
{
	int ret = 0;

	if (in_fd != -1 || out_fd != -1)
	{
		if (!strcmp(args[0], "echo"))
			ret = exec_builtin_with_redir(ft_echo, args, in_fd, out_fd);
		else if (!strcmp(args[0], "cd"))
			ret = exec_builtin_with_redir(ft_cd, args, in_fd, out_fd);
		else if (!strcmp(args[0], "pwd"))
			ret = exec_builtin_with_redir(ft_pwd, args, in_fd, out_fd);
		else if (!strcmp(args[0], "export"))
			ret = exec_builtin_with_redir(ft_export, args, in_fd, out_fd);
		else if (!strcmp(args[0], "unset"))
			ret = exec_builtin_with_redir(ft_unset, args, in_fd, out_fd);
		else if (!strcmp(args[0], "env"))
			ret = exec_builtin_with_redir(ft_env, args, in_fd, out_fd);
	}
	else
	{
		if (!strcmp(args[0], "echo"))
			ret = ft_echo(args);
		else if (!strcmp(args[0], "cd"))
			ret = ft_cd(args);
		else if (!strcmp(args[0], "pwd"))
			ret = ft_pwd(args);
		else if (!strcmp(args[0], "export"))
			ret = ft_export(args);
		else if (!strcmp(args[0], "unset"))
			ret = ft_unset(args);
		else if (!strcmp(args[0], "env"))
			ret = ft_env(args);
	}
	g_shell.last_status = ret;
	return (ret);
}

/* helper implementations were moved to src/main/assigns.c and src/main/dispatch.c */

int main(void)
{
	char *input = NULL;
	char *args[32];
	int in_fd;
	int out_fd;
	char *local_assigns[32];
	char **cmd_envp;

	/* initialize shell globals and signals */
	init_shell();
	start_signals();
	while (1)
	{
		input = get_input_line();
		if (!input)
			break;
		if (is_comment_line(input))
		{
			free(input);
			continue;
		}
		if (is_exit_line(input))
		{
			free(input);
			break;
		}
		/* Now proceed with old parsing for execution */
		split_args(input, args, 32);
		if (has_unmatched_quotes(input))
		{
			g_shell.last_status = 2;
			free(input);
			continue;
		}
		if (args[0] && !strcmp(args[0], "|"))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
			g_shell.last_status = 2;
			free(input);
			continue;
		}
		int last = 0;
		while (args[last])
			last++;
		if (last > 0 && !strcmp(args[last - 1], "|"))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
			g_shell.last_status = 2;
			free(input);
			continue;
		}
		int i = 0;
		while (args[i])
		{
			if (!strcmp(args[i], "|") && args[i + 1] && !strcmp(args[i + 1], "|"))
			{
				ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
				g_shell.last_status = 2;
				free(input);
				continue;
			}
			i++;
		}
		expand_args_inplace(args);
		if (!args[0])
		{
			free(input);
			continue;
		}
		/* collect assignments anywhere in args into local_assigns[] and remove them
		 * but if the command is `export` we should keep assignments as arguments
		 * so ft_export can handle 'export VAR=val' semantics.
		 */
		/* collect and strip local assignments */
		collect_local_assignments(args, local_assigns);
		if (!args[0])
		{
			/* no command: assignments become shell variables */
			apply_assignments_when_no_command(local_assigns);
			continue;
		}
		in_fd = -1;
		out_fd = -1;
		int ret = setup_redirections(args, &in_fd, &out_fd);
		if (ret)
		{
			g_shell.last_status = ret;
			free(input);
			continue;
		}
		/* build per-command envp if local assignments exist */
		cmd_envp = NULL;
		if (local_assigns[0])
			cmd_envp = build_envp_from_local(local_assigns);
		/* debug logging removed */
		if (detect_and_handle_pipeline(args, cmd_envp))
		{
			free(input);
			continue;
		}
		run_command_or_builtin(args, cmd_envp, local_assigns, in_fd, out_fd);
		if (in_fd == -2)
			g_shell.last_status = 1;
		free(input);
	}
	return (g_shell.last_status);
}