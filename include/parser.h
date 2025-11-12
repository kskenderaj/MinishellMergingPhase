/* ************************************************************************** */
/*                                                    // Initialization            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klejdi <klejdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 14:40:09 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/05 20:45:14 by klejdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

// Includes -- BEGIN
#include <stdio.h>
#include <aio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include "garbage_collector.h"

// Forward declarations for command/parser types used across modules
typedef struct s_cmd_node t_cmd_node;
typedef struct s_cmd_list t_cmd_list;
typedef struct s_env_node t_env_node;
typedef struct s_env_list t_env_list;
typedef struct s_file_node t_file_node;
typedef struct s_file_list t_file_list;

// Includes -- END

// Structs -- BEGIN
typedef enum e_toktype
{
	T_IDENTIFIER,
	T_LPAREN,
	T_RPAREN,
	T_OR,
	T_AND,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_PIPE,
	T_HEREDOC,
	T_UNKNOWN,
	T_EOF
} t_toktype;

typedef enum seg_type
{
	SEG_NO_QUOTE,
	SEG_SINGLE,
	SEG_DOUBLE,
} t_seg_type;

typedef struct s_segment
{
	char *value;
	t_seg_type type;
	struct s_segment *next;
} t_segment;

typedef struct s_segment_list
{
	t_segment *head;
	t_segment *tail;
	ssize_t size;
} t_segment_list;

typedef struct s_token
{
	t_toktype type;
	char *value;
	t_segment_list *segment_list;
	struct s_token *next;
} t_token;

typedef struct s_token_list
{
	int syntax_error;
	t_token *head;
	t_token *tail;
	ssize_t size;
	char *input;
} t_token_list;
// Structs -- END

// Lexing checking syntax
// int							main(int argc, char **argv);
bool is_valid_red(char *str, int *i);
bool is_red(char *str, int *i);
bool is_valid_pipe(char *str, int *i);
bool check_tokens(char *str, int i);
bool is_boundary_char(char c);
bool we_have_token(const char *str, int *i);
int scan_quote(const char *str, int i);
bool is_valid_quote(char *str, int *i);
int skip_spaces(char *str, int i);
/* helpers used in token_to_cmd */
bool is_redirection(t_toktype t);
// tokenize
t_token *create_token(t_toktype type, char *val);
int push_token(t_token_list *lst, t_token *token);
int add_token(t_token_list *lst, t_toktype type, char *str, int len);
int handle_word(t_token_list *lst, char *input, int *i);
int tokenize(t_token_list *lst, char *input);
int red_len(char *input, int i);
t_toktype red_type(const char *str, int i);
int handle_quote(char *input, int *i);
int handle_redir(t_token_list *lst, char *input, int *i, int red_len);
int word_end(char *input, int i);
// token to command
int find_segment(t_segment_list *lst, char *str);
// static int quote_segment(t_segment_list *lst, char *str, int *i);
// static int no_quote_segment(t_segment_list *lst, char *str, int *i);
int push_segment(t_segment_list *lst, t_segment *segment);
t_segment *create_segment(char *start, int len, t_seg_type type);

/* Command/list helpers (implemented in parsing/command) */
t_cmd_node *create_cmdnode(void);
void push_cmd(t_cmd_list *lst, t_cmd_node *node);
void create_filenode(char *str, int red_type, t_file_list *filelst);

/* Segment utilities used elsewhere */
void init_segment_lst(t_segment_list *lst);
char *segments_expand(t_segment_list *seglst, t_env_list *envlst, int last_status);
char *expand_env(char *str, t_env_list *env_lst);
char *check_for_env(t_env_list *envlst, const char *str, size_t len);
char *get_expand(char *seg_str, int i, int last_status, t_env_list *envlst);

// debug
void print_tokens(const t_token_list *lst);
void print_segment_list(const t_segment_list *list);
void init_segment_lst(t_segment_list *lst);

// Initialization
void init_token_lst(t_token_list *lst);
void init_cmd_lst(t_cmd_list *lst);
void init_env_lst(t_env_list *lst);

// Parsing
char *look_for_cmd(t_token *token, t_token_list *toklst, t_cmd_list *cmdlst);
void final_token(t_token_list *toklst, t_env_list *envlst, int last_status);

/* ADD THESE MISSING PROTOTYPES */
char *ft_getenv(const char *name, t_env_list *env_list);
void ft_setenv(const char *name, const char *value, t_env_list *env_list);
t_token_list *tokenize_input(char *line);
t_cmd_list *parse_commands(t_token_list *tokens);
void free_token_lst(t_token_list **lst);
void free_cmd_lst(t_cmd_list **lst);
void cleanup_shell(void);
int is_exit_command(char *line);
/* END OF ADDED PROTOTYPES */

#endif
