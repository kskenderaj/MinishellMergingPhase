/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:37:54 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/23 16:38:56 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int	handle_quoted_redir_file(t_token_list *lst, char *input, int *i,
		t_shell_state *shell)
{
	int	start;
	int	next;

	start = *i;
	next = scan_quote(input, *i);
	if (next < 0)
		return (1);
	if (add_token(lst, TK_WORD, input + start, next - start, shell))
		return (1);
	*i = next;
	return (0);
}

int	add_redir_filename(t_token_list *lst, char *input, int *i, t_shell_state *shell)
{
	int	start;
	int	end;

	if (input[*i] == '\'' || input[*i] == '\"')
		return (handle_quoted_redir_file(lst, input, i, shell));
	start = *i;
	end = word_end(input, *i);
	if (end <= start)
		return (1);
	if (add_token(lst, TK_WORD, input + start, end - start, shell) != 0)
		return (1);
	*i = end;
	return (0);
}
