/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 03:08:27 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 04:26:18 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

static int tokenize_core(t_token_list *lst, char *input)
{
    int i;
    int start;
    int rl;

    i = 0;
    while (input[i])
    {
        i = skip_spaces(input, i);
        if (!input[i])
            break;
        if (input[i] == '|')
        {
            if (add_token(lst, T_PIPE, input + i, 1))
                return (1);
            i++;
            continue;
        }
        rl = red_len(input, i);
        if (rl)
        {
            if (handle_redir(lst, input, &i, rl))
                return (1);
            continue;
        }
        start = i;
        i = word_end(input, i);
        if (i < 0)
            return (1);
        if (i > start)
        {
            if (add_token(lst, T_IDENTIFIER, input + start, i - start))
                return (1);
        }
    }
    return (0);
}

t_token_list *tokenize_input(char *line)
{
    t_token_list *tokens;

    if (!line)
        return (NULL);
    tokens = (t_token_list *)malloc(sizeof(t_token_list));
    if (!tokens)
        return (NULL);
    init_token_lst(tokens);
    if (tokenize_core(tokens, line) != 0)
    {
        tokens->syntax_error = 1;
        free(tokens);
        return (NULL);
    }
    return (tokens);
}
