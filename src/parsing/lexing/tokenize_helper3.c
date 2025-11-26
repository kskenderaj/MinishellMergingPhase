/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 16:24:41 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/26 18:17:40 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int handle_pipe_token(t_token_list *lst, char *input, int *i,
                      t_shell_state *shell)
{
    t_tokenize_ctx ctx;

    ctx.lst = lst;
    ctx.input = input + *i;
    ctx.i = i;
    ctx.shell = shell;
    if (add_token(&ctx, TK_PIPE, 1) != 0)
        return (1);
    (*i)++;
    return (0);
}

int handle_redir_token(t_token_list *lst, char *input, int *i,
                       t_shell_state *shell)
{
    t_tokenize_ctx ctx;

    ctx.lst = lst;
    ctx.input = input;
    ctx.i = i;
    ctx.shell = shell;
    return (handle_redir(&ctx, red_len(input, *i)));
}

static int process_token(t_token_list *lst, char *input, int *i,
                         t_shell_state *shell)
{
    *i = skip_spaces(input, *i);
    if (!input[*i])
        return (0);
    if (input[*i] == '|')
        return (handle_pipe_token(lst, input, i, shell));
    else if (red_len(input, *i))
        return (handle_redir_token(lst, input, i, shell));
    else
        return (handle_word(lst, input, i, shell));
}

int tokenize_main(t_token_list *lst, char *input, t_shell_state *shell)
{
    int i;
    int result;

    i = 0;
    while (input[i])
    {
        result = process_token(lst, input, &i, shell);
        if (result < 0)
            return (result);
        if (result == 0 && !input[i])
            break;
        if (result > 0)
            return (result);
    }
    return (0);
}
