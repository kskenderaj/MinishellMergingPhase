/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 03:08:27 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/15 21:24:50 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

// split to keep norm
static int skip_spaces_l(const char *s, int i)
{
    while (s[i] == ' ' || s[i] == '\t')
        i++;
    return (i);
}

static const char *redir_unexpected(const char *s, int i)
{
    int len;
    int j;

    if (s[i + 1] == s[i])
        len = 2;
    else
        len = 1;
    j = skip_spaces_l(s, i + len);
    if (!s[j])
        return ("newline");
    if (s[j] == '<')
        return (s[j + 1] == '<' ? "<<" : "<");
    if (s[j] == '>')
        return (s[j + 1] == '>' ? ">>" : ">");
    return (NULL);
}

static int skip_word_or_quote(const char *s, int i)
{
    if (s[i] == '\'' || s[i] == '"')
    {
        char q = s[i++];
        while (s[i] && s[i] != q)
            i++;
        if (s[i])
            i++;
        return (i);
    }
    while (s[i] && s[i] != ' ' && s[i] != '\t' && s[i] != '|' && s[i] != '<' && s[i] != '>')
        i++;
    return (i);
}

/* Identify the unexpected token to print in the syntax error message. */
static const char *detect_unexpected_token(const char *s)
{
    int i;
    int j;
    const char *r;

    if (!s)
        return ("|");
    i = 0;
    while (s[i])
    {
        i = skip_spaces_l(s, i);
        if (!s[i])
            break;
        if (s[i] == '|')
        {
            j = skip_spaces_l(s, i + 1);
            if (!s[j] || s[j] == '|')
                return ("|");
        }
        else if (s[i] == '<' || s[i] == '>')
        {
            r = redir_unexpected(s, i);
            if (r)
                return (r);
        }
        i = skip_word_or_quote(s, i);
    }
    return ("|");
}

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
    /* fast syntax validation before tokenizing */
    if (!check_tokens(line, 0))
    {
        const char *tok = detect_unexpected_token(line);
        if (tok && ft_strcmp(tok, "newline") == 0)
            ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", STDERR_FILENO);
        else if (tok)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
            ft_putstr_fd((char *)tok, STDERR_FILENO);
            ft_putstr_fd("'\n", STDERR_FILENO);
        }
        else
            ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
        g_shell.last_status = 2;
        return (NULL);
    }
    tokens = (t_token_list *)malloc(sizeof(t_token_list));
    if (!tokens)
        return (NULL);
    init_token_lst(tokens);
    if (tokenize_core(tokens, line) != 0)
    {
        tokens->syntax_error = 1;
        free_token_lst(&tokens);
        return (NULL);
    }
    return (tokens);
}
