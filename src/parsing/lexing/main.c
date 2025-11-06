/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtoumani <jtoumani@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 18:14:59 by jtoumani          #+#    #+#             */
/*   Updated: 2025/09/26 14:25:02 by jtoumani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"
#include "parser.h"
#include "debug.h"

char	*expand_or_not(char *seg_str, t_seg_type seg_type, t_env_list *envlst,
        int last_status, int i);
char	*segments_expand(t_segment_list *seglst, t_env_list *envlst, int last_status);

char	*expand_or_not(char *seg_str, t_seg_type seg_type, t_env_list *envlst,
        int last_status, int i);

static const char	*redir_name(t_toktype t)
{
    if (t == TK_INFILE)
        return ("<");
    if (t == TK_OUTFILE)
        return (">");
    if (t == TK_APPEND)
        return (">>");
    if (t == TK_HEREDOC)
        return ("<<");
    return ("?");
}

static void	print_cmd_node(t_cmd_node *node, int idx)
{
    int			i;
    t_file_node	*f;

    printf("Command #%02d:\n", idx);
    printf("  argv:");
    if (!node->cmd || !node->cmd[0])
        printf(" (empty)\n");
    else
    {
        i = 0;
        while (node->cmd[i])
        {
            printf(" [%02d] \"%s\"", i, node->cmd[i]);
            i++;
        }
        printf("\n");
    }
    printf("  redirs (%zu):", node->files ? node->files->size : 0);
    f = node->files ? node->files->head : NULL;
    if (!f)
        printf(" (none)\n");
    else
    {
        printf("\n");
        while (f)
        {
            printf("    - %s %s\n", redir_name(f->redir_type),
                f->filename ? f->filename : "(null)");
            f = f->next;
        }
    }
    if (node->cmd && node->cmd[0])
    {
        if (node->cmd_type == BUILTIN)
            printf("  type: BUILTIN\n");
        else
            printf("  type: CMD\n");
    }
}

static void	print_cmd_list(t_cmd_list *lst)
{
    t_cmd_node	*cur;
    int			idx;

    if (!lst)
    {
        printf("Cmd list: (null)\n");
        return ;
    }
    printf("Cmd list (size=%zu, syntax_error=%d):\n",
        lst->size, lst->syntax_error);
    if (lst->syntax_error)
    {
        printf("  ❌ Syntax error detected!\n");
        return ;
    }
    cur = lst->head;
    idx = 0;
    while (cur)
    {
        print_cmd_node(cur, idx);
        cur = cur->next;
        idx++;
    }
}

static void	process_line(char *line, t_env_list *envlst, int last_status)
{
    t_token_list	toklst;
    t_cmd_list		cmdlst;

    if (!check_tokens(line, 0))
    {
        printf("❌ Syntax error\n");
        return ;
    }
    init_token_lst(&toklst);
    if (tokenize(&toklst, line) != 0)
    {
        printf("❌ Tokenization failed\n");
        return ;
    }
    final_token(&toklst, envlst, last_status);
    cmdlst.head = NULL;
    cmdlst.tail = NULL;
    cmdlst.size = 0;
    cmdlst.syntax_error = 0;
    look_for_cmd(toklst.head, &toklst, &cmdlst);
    printf("\n");
    print_cmd_list(&cmdlst);
    printf("\n");
}

int	main(int argc, char **argv, char **envp)
{
    t_env_list	envlst;
    char		*line;
    int			last_status;

    (void)argc;
    (void)argv;
    init_env_lst(&envlst);
    get_envs(envp, &envlst);
    last_status = 0;
    while (1)
    {
        line = readline("911TurboS> ");
        if (!line)
            break ;
        if (*line)
        {
            add_history(line);
            printf("%s\n", line);
            process_line(line, &envlst, last_status);
        }
        free(line);
    }
    printf("exit\n");
    return (0);
}