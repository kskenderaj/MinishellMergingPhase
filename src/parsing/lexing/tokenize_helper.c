#include "parser.h"

int	red_len(char *input, int i)
{
	if (!input[i])
		return (0);
	if ((input[i] == '<' && input[i + 1] == '<') || (input[i] == '>' && input[i
		+ 1] == '>'))
	{
		return (2);
	}
	if (input[i] == '<' || input[i] == '>')
		return (1);
	return (0);
}

t_toktype	red_type(const char *str, int i)
{
	if (str[i] == '<' && str[i + 1] == '<')
		return (TK_HEREDOC);
	if (str[i] == '>' && str[i + 1] == '>')
		return (TK_APPEND);
	if (str[i] == '<')
		return (TK_INFILE);
	return (TK_OUTFILE);
}

int handle_quote(char *input, int *i)
{
	int next;
	
	if(input[*i] != '\'' && input[*i] != '\"')
		return 0;
	next = scan_quote(input, *i);
	if(next < 0)
		return 1;
	*i = next;
	return 0;
}

int	word_end(char *input, int i)
{
	int next;
	
	while (input[i] && input[i] != ' ' && input[i] != '\t' && input[i] != '|'
		&& input[i] != '<' && input[i] != '>')
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			next = scan_quote(input, i);
			if (next < 0)
				return (-1);
			i = next;
			}
			else
			i++;
		}
	return (i);
}

int handle_redir(t_token_list *lst, char *input, int *i, int red_len)
{
	int start;
	int next;
	int end;

	if(!red_len)
		return 1;
	if(add_token(lst, red_type(input, *i), input + *i, red_len) != 0)
		return 1;
	*i += red_len;
	*i = skip_spaces(input, *i);
	 if (!input[*i] || input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
		return 1;
	if(input[*i] == '\'' || input[*i] == '\"')
	{
		start = *i;
		next = scan_quote(input, *i);
		if(next < 0)
			return 1;
		if(!add_token (lst, TK_WORD, input + start, next - start))
			return 1;
		*i = next;
		return 0;
	}
	start = *i;
	end = word_end(input, *i);
	if(end <= start)
		return 1;
	if(add_token(lst, TK_WORD, input + start, end - start) != 0)
		return 1;
	*i = end;
	return 0;
}
