
#include "minishell.h"
#include "parser.h"

bool is_redirection(t_toktype t)
{
	return (t == TK_INFILE || t == TK_OUTFILE || t == TK_APPEND || t == TK_HEREDOC);
}

int count_args(t_token *token)
{
	int i;

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

char **create_array(t_token *token, t_cmd_node *cmdnode, int i)
{
	char **cmd_array;

	cmd_array = gc_malloc(sizeof(char *) * ((size_t)count_args(token) + 1));
	if (!cmd_array)
		return (NULL);
	while (token && token->type != TK_PIPE)
	{
		if (is_redirection(token->type))
		{
			if (token->next)
				token = token->next;
			if (token)
				token = token->next;
			continue;
		}
		if (token && token->type == TK_WORD)
		{
			cmd_array[i] = token->value;
			if (i == 0 && cmd_array[0] && is_built_in(cmd_array[0]))
				cmdnode->cmd_type = BUILTIN;
			i++;
		}
		token = token->next;
	}
	return (cmd_array[i] = NULL, cmd_array);
}

char *look_for_cmd(t_token *token, t_token_list *toklst, t_cmd_list *cmdlst)
{
	t_cmd_node *cmdnode;

	token = toklst->head;
	while (token)
	{
		cmdnode = create_cmdnode();
		if (!cmdnode)
			return (NULL);
		cmdnode->cmd = create_array(token, cmdnode, 0);
		if (!cmdnode->cmd || collect_redirs(token, cmdnode) < 0)
		{
			cmdlst->syntax_error = 1;
			return (NULL);
		}
		push_cmd(cmdlst, cmdnode);
		while (token && token->type != TK_PIPE)
			token = token->next;
		if (token && token->type == TK_PIPE)
			token = token->next;
	}
	return (NULL);
}

void final_token(t_token_list *toklst, t_env_list *envlst, int last_status)
{
	t_token *token;
	t_segment_list *seglst;

	if (!toklst)
		return;
	token = toklst->head;
	while (token)
	{
		if (token->type == TK_WORD)
		{
			seglst = gc_malloc(sizeof(*seglst));
			if (!seglst)
				return;
			init_segment_lst(seglst);
			if (find_segment(seglst, token->value))
				token->value = segments_expand(seglst, envlst, last_status);
		}
		token = token->next;
	}
}
