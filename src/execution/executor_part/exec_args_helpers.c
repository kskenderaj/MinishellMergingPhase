/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_args_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:14:26 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/12 02:58:34 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static char *get_next_word(char **str, char delim)
{
    char *scan = *str;
    char *result;
    int total = 0;
    int first_quote = 0; /* 0 = none, '\'' = single, '"' = double */
    int all_quoted_segments = 1;

    /* skip leading delimiters */
    while (*scan == delim)
        scan++;
    if (*scan == '\0')
        return (NULL);

    /* First pass: measure length of the full word (merging quoted parts) */
    char *p = scan;
    while (*p && *p != delim)
    {
        if (*p == '\\')
        {
            /* escaped char: count the next char as literal (if present) */
            if (p[1])
                total++, p += 2;
            else
                p++;
        }
        else if (*p == '\'' || *p == '"')
        {
            char q = *p;
            if (first_quote == 0)
                first_quote = q;
            /* scan inside quote */
            p++;
            char *start = p;
            while (*p && *p != q)
                p++;
            total += (int)(p - start);
            if (*p == q)
                p++;
        }
        else
        {
            all_quoted_segments = 0;
            total++;
            p++;
        }
    }

    if (total == 0)
    {
        /* Produce empty token (e.g., "" or '') so argument positions stay consistent */
        char *empty = gc_malloc(1);
        if (!empty)
            return (NULL);
        empty[0] = '\0';
        *str = scan;
        return (empty);
    }

    /* allocate space (extra byte for optional marker) */
    int need_marker;
    if (all_quoted_segments && first_quote != 0)
        need_marker = 1;
    else
        need_marker = 0;
    result = gc_malloc((size_t)total + need_marker + 1);
    if (!result)
        return (NULL);

    char *out = result;
    if (need_marker)
    {
        if (first_quote == '\'')
            out[0] = '\x01';
        else
            out[0] = '\x02';
        out++;
    }

    /* Second pass: copy content into result (skip surrounding quotes) */
    p = scan;
    int idx = 0;
    while (*p && *p != delim)
    {
        if (*p == '\\')
        {
            /* escaped character outside single-quote: copy next char as literal */
            if (p[1])
            {
                out[idx++] = p[1];
                p += 2;
            }
            else
                p++;
        }
        else if (*p == '\'' || *p == '"')
        {
            char q = *p;
            p++;
            while (*p && *p != q)
            {
                out[idx++] = *p;
                p++;
            }
            if (*p == q)
                p++;
        }
        else
        {
            out[idx++] = *p;
            p++;
        }
    }
    out[idx] = '\0';

    *str = p;
    return (result);
}

void split_args(char *input, char **args, int max_args)
{
    int i = 0;
    char *ptr = input;

    while (i < max_args - 1)
    {
        char *w = get_next_word(&ptr, ' ');
        if (!w)
            break;
        args[i++] = w;
    }
    args[i] = NULL;
}

void shift_left_by(char **args, int start, int by)
{
    int j = start;

    while (args[j + by])
    {
        args[j] = args[j + by];
        j++;
    }
    /* terminate once; callers must not rely on multiple trailing NULLs */
    args[j] = NULL;
}
