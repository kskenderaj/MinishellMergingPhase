/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_main_utilities.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:05:38 by klejdi            #+#    #+#             */
/*   Updated: 2025/10/24 16:03:40 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "builtins.h"
#include <stdio.h>

// Simple argument splitter
void split_args(char *input, char **args, int max_args)
{
    int i = 0;
    char *token = strtok(input, " ");
    while (token && i < max_args - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

/* shift arguments left by `by` positions starting at index `start` */
static void shift_left_by(char **args, int start, int by)
{
    int j = start;
    while (args[j + by])
    {
        args[j] = args[j + by];
        j++;
    }
    args[j] = NULL;
    if (by == 2)
        args[j + 1] = NULL;
}

/* Handle attached operators such as ">>file" ">file" "<<DELIM" "<file".
   Returns 0 on handled, 1 on error, 2 if not an attached operator. */
static int handle_attached_operators(char **args, int *idx, int *in_fd, int *out_fd, char *tok)
{
    /* dispatch to small helpers to keep this function short */
    if (strncmp(tok, ">>", 2) == 0)
        return ((int)(intptr_t)({
            /* attached append */
            char *fname = (tok[2] != '\0') ? tok + 2 : args[*idx + 1];
            if (!fname)
                return (int)1;
            if (*out_fd != -1)
                close(*out_fd);
            *out_fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (tok[2] != '\0')
                shift_left_by(args, *idx, 1);
            else
                shift_left_by(args, *idx, 2);
            0;
        }));
    if (strncmp(tok, "<<", 2) == 0)
        return ((int)(intptr_t)({
            /* attached heredoc */
            char *delim = (tok[2] != '\0') ? tok + 2 : args[*idx + 1];
            if (!delim)
                return (int)1;
            if (*in_fd != -1)
                close(*in_fd);
            *in_fd = exec_heredoc(delim);
            if (tok[2] != '\0')
                shift_left_by(args, *idx, 1);
            else
                shift_left_by(args, *idx, 2);
            0;
        }));
    if (tok[0] == '>' && tok[1] != '>')
        return ((int)(intptr_t)({
            /* attached outfile */
            char *fname = (tok[1] != '\0') ? tok + 1 : args[*idx + 1];
            if (!fname)
                return (int)1;
            if (*out_fd != -1)
                close(*out_fd);
            *out_fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (tok[1] != '\0')
                shift_left_by(args, *idx, 1);
            else
                shift_left_by(args, *idx, 2);
            0;
        }));
    if (tok[0] == '<' && tok[1] != '<')
        return ((int)(intptr_t)({
            /* attached infile */
            char *fname = (tok[1] != '\0') ? tok + 1 : args[*idx + 1];
            if (!fname)
                return (int)1;
            if (*in_fd != -1)
                close(*in_fd);
            *in_fd = open(fname, O_RDONLY);
            if (tok[1] != '\0')
                shift_left_by(args, *idx, 1);
            else
                shift_left_by(args, *idx, 2);
            0;
        }));
    return (2);
}

/* Handle separated operator tokens like ">" "file" or "<<" "DELIM".
   Returns 0 on handled, 1 on error, 2 if not a separated operator. */
static int handle_separated_operators(char **args, int *i, int *in_fd, int *out_fd)
{
    if (strcmp(args[*i], ">") == 0 && args[*i + 1])
    {
        if (*out_fd != -1)
        {
            close(*out_fd);
            *out_fd = -1;
        }
        *out_fd = open(args[*i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        args[*i] = NULL;
        args[*i + 1] = NULL;
        shift_left_by(args, *i, 2);
        return (0);
    }
    if (strcmp(args[*i], ">>") == 0 && args[*i + 1])
    {
        if (*out_fd != -1)
        {
            close(*out_fd);
            *out_fd = -1;
        }
        *out_fd = open(args[*i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        args[*i] = NULL;
        args[*i + 1] = NULL;
        shift_left_by(args, *i, 2);
        return (0);
    }
    if (strcmp(args[*i], "<") == 0 && args[*i + 1])
    {
        if (*in_fd != -1)
        {
            close(*in_fd);
            *in_fd = -1;
        }
        *in_fd = open(args[*i + 1], O_RDONLY);
        args[*i] = NULL;
        args[*i + 1] = NULL;
        shift_left_by(args, *i, 2);
        return (0);
    }
    if (strcmp(args[*i], "<<") == 0 && args[*i + 1])
    {
        if (*in_fd != -1)
        {
            close(*in_fd);
            *in_fd = -1;
        }
        *in_fd = exec_heredoc(args[*i + 1]);
        args[*i] = NULL;
        args[*i + 1] = NULL;
        shift_left_by(args, *i, 2);
        return (0);
    }
    return (2);
}

int setup_redirections(char **args, int *in_fd, int *out_fd)
{
    int i;
    *in_fd = -1;
    *out_fd = -1;
    i = 0;
    while (args[i])
    {
        char *tok = args[i];
        /* Detect separated redirection with missing target: '>' '<' '>>' '<<' followed by nothing */
        if ((strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0 || strcmp(tok, "<") == 0 || strcmp(tok, "<<") == 0) && args[i + 1] == NULL)
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `newline`\n");
            return (1);
        }
        int res;
        res = handle_attached_operators(args, &i, in_fd, out_fd, tok);
        if (res != 2)
        {
            if (res != 0)
                return (1);
            continue;
        }
        res = handle_separated_operators(args, &i, in_fd, out_fd);
        if (res != 2)
        {
            if (res != 0)
                return (1);
            continue;
        }
        i++;
    }
    if ((*in_fd != -1 && *in_fd < 0) || (*out_fd != -1 && *out_fd < 0))
        return (1);
    return (0);
}

/* exec_external removed from this file to avoid duplicate symbol.
    Implementation lives in exec_external_handler.c */