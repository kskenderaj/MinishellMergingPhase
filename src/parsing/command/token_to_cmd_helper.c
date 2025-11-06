#include "minishell.h"
#include "parser.h"

bool is_built_in(char *str)
{
	if (!str)
		return false;
	if (ft_strncmp(str, "echo", 5) == 0)
		return true;
	if (ft_strncmp(str, "cd", 3) == 0)
		return true;
	if (ft_strncmp(str, "pwd", 4) == 0)
		return true;
	if (ft_strncmp(str, "export", 7) == 0)
		return true;
	if (ft_strncmp(str, "unset", 6) == 0)
		return true;
	if (ft_strncmp(str, "env", 4) == 0)
		return true;
	if (ft_strncmp(str, "exit", 5) == 0)
		return true;
	return false;
}

int collect_redirs(t_token *token, t_cmd_node *cmdnode)
{
	while (token && token->type != TK_PIPE)
	{
		if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != TK_WORD)
				return -1;
			create_filenode(token->next->value, token->type, cmdnode->files);
			token = token->next;
			if (token)
				token = token->next;
			continue;
		}
		token = token->next;
	}
	return 0;
}

// void	final_token(t_token_list *toklst, t_env_list *envlst, int last_status)
// {
// 	t_token			*token;
// 	t_segment_list	*seglst;

// 	if (!toklst)
// 		return ;
// 	token = toklst->head;
// 	while (token)
// 	{
// 		if (token->type == TK_WORD)
// 		{
// 			seglst = gc_malloc(sizeof(*seglst));
// 			if(!seglst)
// 				return;
// 			init_segment_lst(seglst);
// 			if (find_segment(seglst, token->value))
// 				token->value = segments_expand(seglst, envlst, last_status);
// 		}
// 		token = token->next;
// 	}
// }
