/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/18 19:56:37 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

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

static int	handle_split_word(char **cmd_array, char *value, int *i)
{
	char	**words;
	int		j;

	words = split_on_spaces(value);
	if (!words)
		return (-1);
	j = 0;
	while (words[j])
	{
		cmd_array[*i] = words[j];
		(*i)++;
		j++;
	}
	return (0);
}

char	**create_array(t_token *token, t_cmd_node *cmdnode, int i)
{
	char		**cmd_array;
	t_env_node	*env_node;

	cmd_array = gc_malloc(sizeof(char *) * ((size_t)count_args(token) + 1));
	if (!cmd_array)
		return (NULL);
	while (token && token->type != TK_PIPE)
	{
		if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != TK_WORD)
				return (NULL);
			if (token->next)
				token = token->next;
			if (token)
				token = token->next;
			continue ;
		}
		if (token && token->type == TK_WORD)
		{
			if (i == 0 && is_valid_env_assignment(token->value))
			{
				env_node = gc_malloc(sizeof(t_env_node));
				if (env_node && find_key(token->value, env_node)
					&& find_value(token->value, env_node))
					push_env(cmdnode->env, env_node);
				token = token->next;
				continue ;
			}
			if (i == 0 && token->value && token->segment_list
				&& should_split(token->segment_list) && ft_strchr(token->value,
					' '))
			{
				if (handle_split_word(cmd_array, token->value, &i) < 0)
					return (NULL);
			}
			else
			{
				cmd_array[i] = token->value;
				i++;
			}
			if (i > 0 && cmd_array[0] && is_built_in(cmd_array[0]))
				cmdnode->cmd_type = BUILTIN;
		}
		token = token->next;
	}
	return (cmd_array[i] = NULL, cmd_array);
}

char	*look_for_cmd(t_token *token, t_token_list *toklst, t_cmd_list *cmdlst)
{
	t_cmd_node	*cmdnode;

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



int	token_to_cmd(t_token_list *toklst, t_cmd_list *cmdlst, t_env_list *envlst,
		int last_status)
{
	if (!toklst || !cmdlst || !envlst)
		return (1);
	if (validate_pipe_syntax(toklst))
	{
		cmdlst->syntax_error = 1;
		return (1);
	}
	final_token(toklst, envlst, last_status);
	look_for_cmd(toklst->head, toklst, cmdlst);
	if (cmdlst->syntax_error)
		return (1);
	return (0);
}

void	final_token(t_token_list *toklst, t_env_list *envlst, int last_status)
{
	t_token			*token;
	t_segment_list	*seglst;
	int				skip_next;

	if (!toklst)
		return ;
	token = toklst->head;
	skip_next = 0;
	while (token)
	{
		if (skip_next)
		{
			skip_next = 0;
			token = token->next;
			continue ;
		}
		if (token->type == TK_HEREDOC)
			skip_next = 1;
		else if (token->type == TK_WORD)
		{
			seglst = gc_malloc(sizeof(*seglst));
			if (!seglst)
				return ;
			init_segment_lst(seglst);
			if (find_segment(seglst, token->value))
			{
				token->segment_list = seglst;
				token->value = segments_expand(seglst, envlst, last_status);
			}
		}
		token = token->next;
	}
}
