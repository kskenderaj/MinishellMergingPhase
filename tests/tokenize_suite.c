#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static const char *toktype_name[] = {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static void collect_tokens(t_token_list *lst, char ***out_tokens, int *out_count)
{
    t_token *t = lst->head;
    int cap = 16;
    int n = 0;
    char **arr = malloc(sizeof(char *) * cap);
    while (t)
    {
        char buf[1024];
        const char *val = t->value ? t->value : "(null)";
        snprintf(buf, sizeof(buf), "%s:%s", (t->type >= 0 && t->type <= TK_PIPE) ? toktype_name[t->type] : "UNKNOWN", val);
        if (n + 1 >= cap)
        {
            cap *= 2;
            arr = realloc(arr, sizeof(char *) * cap);
        }
        arr[n++] = strdup(buf);
        t = t->next;
    }
    *out_tokens = arr;
    *out_count = n;
}

static void free_tokens(char **arr, int n)
{
    for (int i = 0; i < n; ++i)
        free(arr[i]);
    free(arr);
}

static int run_case_impl(const char *input, const char **expected, int expected_count)
{
    t_token_list lst;
    lst.head = NULL;
    lst.tail = NULL;
    lst.size = 0;
    lst.syntax_error = 0;

    int res = tokenize(&lst, (char *)input);
    if (res != 0)
    {
        printf("FAIL: tokenize returned %d for input: %s\n", res, input);
        return 1;
    }
    char **got;
    int gotc;
    collect_tokens(&lst, &got, &gotc);
    if (gotc != expected_count)
    {
        printf("FAIL: token count mismatch for '%s' (got %d, expected %d)\n", input, gotc, expected_count);
        for (int i = 0; i < gotc; ++i)
            printf("  got[%d] = %s\n", i, got[i]);
        for (int i = 0; i < expected_count; ++i)
            printf("  exp[%d] = %s\n", i, expected[i]);
        free_tokens(got, gotc);
        return 1;
    }
    for (int i = 0; i < gotc; ++i)
    {
        if (strcmp(got[i], expected[i]) != 0)
        {
            printf("FAIL: token mismatch for '%s' at index %d\n  got: %s\n  exp: %s\n", input, i, got[i], expected[i]);
            free_tokens(got, gotc);
            return 1;
        }
    }
    free_tokens(got, gotc);
    return 0;
}

int main(void)
{
    struct case_t
    {
        const char *in;
        const char **out;
        int outc;
    };

    static const char *c0[] = {"TK_WORD:echo", "TK_WORD:hi"};
    static const char *c1[] = {"TK_WORD:ls", "TK_WORD:-l", "TK_PIPE:|", "TK_WORD:wc", "TK_WORD:-l"};
    static const char *c2[] = {"TK_WORD:echo", "TK_WORD:'a b'", "TK_WORD:\"c d\""};
    static const char *c3[] = {"TK_WORD:cat", "TK_OUTFILE:>", "TK_WORD:out"};
    static const char *c4[] = {"TK_WORD:cmd", "TK_APPEND:>>", "TK_WORD:out"};
    static const char *c5[] = {"TK_WORD:echo", "TK_WORD:\"inside $VAR\""};
    static const char *c7[] = {"TK_WORD:cat", "TK_INFILE:<", "TK_WORD:infile"};
    static const char *c8[] = {"TK_WORD:cat", "TK_HEREDOC:<<", "TK_WORD:EOF"};
    static const char *c9[] = {"TK_WORD:cmd", "TK_INFILE:<", "TK_WORD:in", "TK_OUTFILE:>", "TK_WORD:out"};
    static const char *c10[] = {"TK_WORD:echo", "TK_WORD:a", "TK_OUTFILE:>", "TK_WORD:out", "TK_PIPE:|", "TK_WORD:wc", "TK_WORD:-c"};
    static const char *c11[] = {"TK_WORD:a", "TK_PIPE:|", "TK_WORD:b", "TK_PIPE:|", "TK_WORD:c"};
    static const char *c12[] = {"TK_WORD:printf", "TK_WORD:\"line1\\nline2\""};
    static const char *c13[] = {"TK_WORD:export", "TK_WORD:KEY=VAL"};
    static const char *c14[] = {"TK_WORD:echo", "TK_WORD:$HOME"};
    static const char *c15[] = {"TK_WORD:ls", "TK_OUTFILE:>", "TK_WORD:out"};
    static const char *c16[] = {"TK_WORD:cmd", "TK_OUTFILE:>", "TK_WORD:out1", "TK_APPEND:>>", "TK_WORD:out2", "TK_INFILE:<", "TK_WORD:in"};

    struct case_t cases[] = {
        {"echo hi", c0, 2},
        {"ls -l | wc -l", c1, 5},
        {"echo 'a b' \"c d\"", c2, 3},
        {"cat > out", c3, 3},
        {"cmd >> out", c4, 3},
        {"echo \"inside $VAR\"", c5, 2},
        {"cat < infile", c7, 3},
        {"cat << EOF", c8, 3},
        {"cmd < in > out", c9, 5},
        {"echo a > out | wc -c", c10, 7},
        {"a | b | c", c11, 5},
        {"printf \"line1\\nline2\"", c12, 2},
        {"export KEY=VAL", c13, 2},
        {"echo $HOME", c14, 2},
        {"ls>out", c15, 3},
        {"cmd >out1 >>out2 <in", c16, 7},
        {NULL, NULL, 0}};

    int failures = 0;
    int total = 0;
    for (int i = 0; cases[i].in != NULL; ++i)
    {
        printf("Test %d: %s\n", i, cases[i].in);
        total++;
        if (run_case_impl(cases[i].in, cases[i].out, cases[i].outc) != 0)
        {
            failures++;
        }
    }

    // check unterminated behavior specifically
    printf("Test unterminated: echo 'unterminated\n");
    {
        t_token_list lst;
        lst.head = NULL;
        lst.tail = NULL;
        lst.size = 0;
        lst.syntax_error = 0;
        int r = tokenize(&lst, "echo 'unterminated");
        if (r == 0)
        {
            printf("FAIL: expected tokenize to fail for unterminated quote but it returned 0\n");
            failures++;
        }
        total++;
    }

    if (failures == 0)
    {
        printf("ALL TESTS PASSED\n");
    }
    else
    {
        printf("%d test(s) failed\n", failures);
    }

    // Print a compact JSON summary for CI parsing
    printf("{\"total\":%d,\"failures\":%d}\n", total, failures);

    return failures == 0 ? 0 : 1;
}

else
{
    printf("%d test(s) failed\n", failures);
    return 1;
}
}
