
#include "minishell.h"
#include "parser.h"

void init_segment_lst(t_segment_list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;
}

void	init_token_lst(t_token_list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;
	lst->syntax_error = 0;
}

void init_env_lst(t_env_list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;
}
void init_cmd_lst(t_cmd_list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;
}
void init_files_lst(t_file_list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;

}
