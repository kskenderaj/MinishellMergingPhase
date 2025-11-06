#include "debug.h"
#include "minishell.h"

static const char *tok_name(t_toktype t)
{
    switch (t)
    {
    case TK_WORD:
        return "WORD";
    case TK_PIPE:
        return "PIPE";
    case TK_INFILE:
        return "INFILE";
    case TK_OUTFILE:
        return "OUTFILE";
    case TK_APPEND:
        return "APPEND";
    case TK_HEREDOC:
        return "HEREDOC";
    default:
        return "UNKNOWN";
    }
}

void print_tokens(const t_token_list *lst)
{
    const t_token *cur = lst ? lst->head : NULL;
    int i = 0;

    printf("%sTokens (%zd):%s\n", COLOR_YELLOW, lst ? lst->size : 0, COLOR_RESET);
    while (cur)
    {
        printf("  [%02d] %-7s value: %s%s%s\n",
               i++, tok_name(cur->type),
               COLOR_GREEN, cur->value ? cur->value : "(null)", COLOR_RESET);
        cur = cur->next;
    }
}

static const char *seg_name(t_seg_type t)
{
    switch (t)
    {
    case SEG_NO_QUOTE:
        return "NO_QUOTE";
    case SEG_SINGLE:
        return "SINGLE";
    case SEG_DOUBLE:
        return "DOUBLE";
    default:
        return "SEG?";
    }
}

void print_segment_list(const t_segment_list *list)
{
    const t_segment *s = list ? list->head : NULL;
    int i = 0;

    printf("%sSegments (%zd):%s\n", COLOR_YELLOW, list ? list->size : 0, COLOR_RESET);
    while (s)
    {
        printf("  [%02d] %-9s value: %s%s%s\n",
               i++, seg_name(s->type),
               COLOR_GREEN, s->value ? s->value : "(null)", COLOR_RESET);
        s = s->next;
    }
}

void print_env_list(t_env_list *lst)
{
    t_env_node *n;

    if (!lst)
    {
        printf("env list NULL\n");
        return;
    }
    n = lst->head;
    printf("Env list (size=%zd):\n", lst->size);
    while (n)
    {
        printf("  %s=%s\n", n->key ? n->key : "(null)", n->value ? n->value : "(null)");
        n = n->next;
    }
}