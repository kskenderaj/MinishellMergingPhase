/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_to_cmd_helper.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 00:00:00 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/24 14:38:08 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

bool	is_built_in(char *str)
{
	if (!str)
		return (false);
	if (ft_strncmp(str, "echo", 5) == 0)
		return (true);
	if (ft_strncmp(str, "cd", 3) == 0)
		return (true);
	if (ft_strncmp(str, "pwd", 4) == 0)
		return (true);
	if (ft_strncmp(str, "export", 7) == 0)
		return (true);
	if (ft_strncmp(str, "unset", 6) == 0)
		return (true);
	if (ft_strncmp(str, "env", 4) == 0)
		return (true);
	if (ft_strncmp(str, "exit", 5) == 0)
		return (true);
	return (false);
}

int	collect_redirs(t_token *token, t_cmd_node *cmdnode, t_shell_state *shell)
{
	while (token && token->type != TK_PIPE)
	{
		if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != TK_WORD)
				return (-1);
			create_filenode(token->next->value, token->type, cmdnode->files,
				shell);
			token = token->next;
			if (token)
				token = token->next;
			continue ;
		}
		token = token->next;
	}
	return (0);
}

bool	is_redirection(t_toktype t)
{
	return (t == TK_INFILE || t == TK_OUTFILE || t == TK_APPEND
		|| t == TK_HEREDOC);
}

int	validate_pipe_syntax(t_token_list *toklst)
{
	t_token	*token;
	t_token	*prev;

	if (!toklst || !toklst->head)
		return (0);
	token = toklst->head;
	prev = NULL;
	if (token->type == TK_PIPE)
		return (1);
	while (token)
	{
		if (token->type == TK_PIPE)
		{
			if (prev && prev->type == TK_PIPE)
				return (1);
			if (!token->next)
				return (1);
			if (token->next && token->next->type == TK_PIPE)
				return (1);
		}
		prev = token;
		token = token->next;
	}
	return (0);
}

int	is_valid_env_assignment(char *str)
{
	int		i;
	char	*eq;

	if (!str || !*str)
		return (0);
	eq = ft_strchr(str, '=');
	if (!eq || eq == str)
		return (0);
	if (!ft_isalpha((unsigned char)str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str + i < eq)
	{
		if (!ft_isalnum((unsigned char)str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}
