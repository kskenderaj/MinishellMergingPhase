/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assigns.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 19:57:32 by klejdi            #+#    #+#             */
/*   Updated: 2025/11/11 19:57:33 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main_helpers.h"
#include "executor.h"

static int is_assignment(const char *s)
{
    const char *p;

    if (!s || !*s)
        return (0);
    p = s;
    if (!(ft_isalpha((unsigned char)*p) || *p == '_'))
        return (0);
    p++;
    while (*p && *p != '=')
    {
        if (!(ft_isalnum((unsigned char)*p) || *p == '_'))
            return (0);
        p++;
    }
    return (*p == '=');
}

static char *collect_assignment(char *s)
{
    if (!s)
        return (NULL);
    return (gc_strdup(s));
}

void collect_local_assignments(char **args, char **local_assigns)
{
    int la_i = 0;
    int r = 0;
    int w = 0;
    int is_export_cmd = (args[0] && !strcmp(args[0], "export"));

    if (is_export_cmd)
    {
        local_assigns[0] = NULL;
        return;
    }
    while (args[r])
    {
        if (is_assignment(args[r]))
        {
            local_assigns[la_i++] = collect_assignment(args[r]);
            r++;
            continue;
        }
        args[w++] = args[r++];
    }
    args[w] = NULL;
    local_assigns[la_i] = NULL;
}

void apply_assignments_when_no_command(char **local_assigns)
{
    char *eq;
    char *key;
    char *val;

    for (int k = 0; local_assigns[k]; ++k)
    {
        eq = ft_strchr(local_assigns[k], '=');
        if (!eq)
            continue;
        key = gc_substr(local_assigns[k], 0, (unsigned int)(eq - local_assigns[k]));
        val = eq + 1;
        if (key)
        {
            if (!val)
                val = "";
            setenv(key, val, 1);
        }
    }
}
