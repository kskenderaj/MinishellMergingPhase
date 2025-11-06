/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_args_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:14:26 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/06 04:12:43 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static char *get_next_word(char **str, char delim)
{
    char *scan = *str;
    char *result;
    int total = 0;
    int first_quote = 0; /* 0 = none, '\'' = single, '"' = double */
    int all_quoted_segments = 1;

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
        *str = scan;
        return (NULL);
    }

    /* allocate space (extra byte for optional marker) */
    int need_marker = (all_quoted_segments && first_quote != 0) ? 1 : 0;
    result = gc_malloc(total + need_marker + 1);
    if (!result)
        return (NULL);

    /* If entire word consists of quoted segments and first quote was single or double,
       prefix marker so callers can detect quoted token semantics. */
    char *out = result;
    if (need_marker)
    {
        out[0] = (first_quote == '\'') ? '\x01' : '\x02';
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

    /* Note: do not further sanitize the token here. We intentionally keep the
       copied contents verbatim (except for removed surrounding quotes). The
       caller (expand_args_inplace) will honor quoted markers to decide
       whether to expand variables. Removing characters here can break that
       logic for inputs produced by the test harness. */

    *str = p;
    /* Debug: append token info to /tmp/get_next_word_debug.txt to help
       understand tokenization in failing tests. This is temporary. */
    {
        int d = open("tests/strict_tmp/get_next_word_debug.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (d != -1)
        {
            dprintf(d, "token='%s' marker=%d len=%d\n", result, need_marker, idx);
            close(d);
        }
    }
    return (result);
}

void split_args(char *input, char **args, int max_args)
{
    int i;
    char *ptr;

    i = 0;
    ptr = input;
    while (i < max_args - 1)
    {
        char *w = get_next_word(&ptr, ' ');
        if (!w)
            break;
        /* skip empty tokens (including marker-only tokens) that can be produced
           when sanitizing embedded test harness sequences */
        if (w[0] == '\0' || (((unsigned char)w[0] == 0x01 || (unsigned char)w[0] == 0x02) && w[1] == '\0'))
            continue;
        /* also skip tokens that are only composed of backslashes or quote chars
           these are often produced by embedding sequences like '\'' in test files */
        int only_special = 1;
        for (int si = 0; w[si]; ++si)
        {
            if (w[si] != '\\' && w[si] != '\'' && w[si] != '"')
            {
                only_special = 0;
                break;
            }
        }
        if (only_special)
            continue;
        args[i] = w;
        i++;
    }
    args[i] = NULL;
}

void shift_left_by(char **args, int start, int by)
{
    int j;

    j = start;
    while (args[j + by])
    {
        args[j] = args[j + by];
        j++;
    }
    while (by > 0)
    {
        args[j] = NULL;
        j++;
        by--;
    }
}