#include "libft/libft.h"

/* Minimaler Wrapper für parse_test */
char	*gc_substr(char *s, unsigned int start, size_t len)
{
    if (!s)
        return (NULL);
    return (ft_substr((const char *)s, start, len));
}