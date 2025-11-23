/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/23 16:36:54 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"
#include "executor.h"

int	count_args(t_token *token)
{
	int	i;

	i = 0;
	while (token && token->type != TK_PIPE)
	{
		if (token->type == TK_WORD)
			i++;
		else if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != TK_WORD)
				return (-1);
			token = token->next;
		}
		token = token->next;
	}
	return (i);
}

char	**create_array(t_token *token, t_cmd_node *cmdnode, int i,
	t_shell_state *shell)
{
	char	**cmd_array;

	cmd_array = gc_malloc(shell->gc, sizeof(char *) * ((size_t)count_args(token)
			+ 1));
	if (!cmd_array)
		return (NULL);
	if (process_tokens_to_array(token, cmdnode, cmd_array, &i, shell) < 0)
		return (NULL);
	cmd_array[i] = NULL;
	return (cmd_array);
}

char	*look_for_cmd(t_token *token, t_token_list *toklst, t_cmd_list *cmdlst,
	t_shell_state *shell)
{
	t_cmd_node	*cmdnode;

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

int	token_to_cmd(t_token_list *toklst, t_cmd_list *cmdlst, t_env_list *envlst,
		int last_status, t_shell_state *shell)
{
	if (!toklst || !cmdlst || !envlst)
		return (1);
	if (validate_pipe_syntax(toklst))
	{
		cmdlst->syntax_error = 1;
		return (1);
	}
	final_token(toklst, envlst, last_status, shell);
	look_for_cmd(toklst->head, toklst, cmdlst, shell);
	if (cmdlst->syntax_error)
		return (1);
	return (0);
}

void	final_token(t_token_list *toklst, t_env_list *envlst, int last_status,
	t_shell_state *shell)
{
	t_token	*token;
	int		skip_next;

	if (!toklst)
		return ;
	token = toklst->head;
	skip_next = 0;
	while (token)
	{
		if (process_single_token(token, &skip_next, envlst, last_status,
				shell) < 0)
			return ;
		token = token->next;
	}
}