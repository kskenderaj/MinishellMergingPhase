#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

static const char *toktype_name[] = {
    "TK_WORD",
    "TK_BUILTIN",
    "S_QUOTES",
    "D_QUOTES",
    "TK_INFILE",
    "TK_OUTFILE",
    "TK_HEREDOC",
    "TK_APPEND",
    "TK_PIPE",
};

static void dump_tokens(t_token_list *lst)
{
    t_token *t = lst->head;
    int i = 0;
    while (t)
    {
        printf("[%d] %s : '%s'\n", i++, (t->type >= 0 && t->type <= TK_PIPE) ? toktype_name[t->type] : "UNKNOWN", t->value ? t->value : "(null)");
        t = t->next;
    }
}

int main(void)
{
    const char *samples[] = {
        "echo hi",
        "ls -l | wc -l",
        "echo 'a b' \"c d\"",
        "cat > out",
        "cmd >> out",
        "echo \"inside $VAR\"",
        "echo 'unterminated",
        NULL};

    for (int s = 0; samples[s]; ++s)
    {
        char *input = (char *)samples[s];
        t_token_list lst;
        lst.head = NULL;
        lst.tail = NULL;
        lst.size = 0;
        lst.syntax_error = 0;
        printf("\nInput: %s\n", input);
        int res = tokenize(&lst, input);
        if (res != 0)
        {
            printf("tokenize: FAILED (%d)\n", res);
            continue;
        }
        dump_tokens(&lst);
    }
    return 0;
}
