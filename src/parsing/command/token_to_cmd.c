/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 16:21:53 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"
#include "parser.h"

char **create_array(t_token *token, t_cmd_node *cmdnode, int i,
					t_shell_state *shell)
{
	char **cmd_array;
	t_token_process_ctx ctx;

	cmd_array = gc_malloc(shell->gc, sizeof(char *) * ((size_t)count_args(token) + 1));
	if (!cmd_array)
		return (NULL);
	ctx.token = token;
	ctx.cmdnode = cmdnode;
	ctx.cmd_array = cmd_array;
	ctx.i = &i;
	ctx.shell = shell;
	if (process_tokens_to_array(&ctx) < 0)
		return (NULL);
	cmd_array[i] = NULL;
	return (cmd_array);
}

char *look_for_cmd(t_token *token, t_token_list *toklst, t_cmd_list *cmdlst,
				   t_shell_state *shell)
{
	t_cmd_node *cmdnode;

	token = toklst->head;
	while (token)
	{
		cmdnode = create_cmdnode(shell);
		if (!cmdnode)
			return (NULL);
		cmdnode->cmd = create_array(token, cmdnode, 0, shell);
		if (!cmdnode->cmd || collect_redirs(token, cmdnode, shell) < 0)
		{
			cmdlst->syntax_error = 1;
			return (NULL);
		}
		push_cmd(cmdlst, cmdnode);
		token = skip_to_next_pipe(token);
	}
	return (NULL);
}

static int process_cmdlst(t_token_list *toklst, t_cmd_list *cmdlst,
						  t_shell_state *shell)
{
	final_token(toklst, shell);
	look_for_cmd(toklst->head, toklst, cmdlst, shell);
	if (cmdlst->syntax_error)
		return (1);
	return (0);
}

int token_to_cmd(t_token_list *toklst, t_cmd_list *cmdlst,
				 t_shell_state *shell)
{
	if (!toklst || !cmdlst || !shell->env)
		return (1);
	if (validate_pipe_syntax(toklst))
	{
		cmdlst->syntax_error = 1;
		return (1);
	}
	return (process_cmdlst(toklst, cmdlst, shell));
}

void final_token(t_token_list *toklst, t_shell_state *shell)
{
	t_token *token;
	int skip_next;

	if (!toklst)
		return;
	token = toklst->head;
	skip_next = 0;
	while (token)
	{
		if (process_single_token(token, &skip_next, shell) < 0)
			return;
		token = token->next;
	}
}
