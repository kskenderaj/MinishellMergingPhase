#include <stdlib.h>
#include "libft/libft.h"

/* Minimaler GC-Shim nur für parse_test */

void	*gc_malloc(size_t size)
{
    void	*p;

    p = malloc(size);
    return (p);
}

char	*gc_strdup(const char *s)
{
    if (!s)
        return (NULL);
    return (ft_strdup(s));
}

char	*gc_strjoin(const char *s1, const char *s2)
{
    if (!s1 || !s2)
        return (NULL);
    return (ft_strjoin((char *)s1, (char *)s2));
}

char	*gc_substr(char *s, unsigned int start, size_t len)
{
    if (!s)
        return (NULL);
    return (ft_substr((char const *)s, start, len));
}

char	*gc_itoa(int n)
{
    return (ft_itoa(n));
}