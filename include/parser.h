/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kskender <kskender@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 14:40:09 by jtoumani          #+#    #+#             */
/*   Updated: 2025/11/19 20:16:33 by kskender         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

// Includes -- BEGIN
# include "garbage_collector.h"
# include <aio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdbool.h>
# include <stdio.h>

// Environment structures - FULL DEFINITIONS
typedef struct s_env_node
{
	char				*key;
	char				*value;
	struct s_env_node	*next;
}						t_env_node;

typedef struct s_env_list
{
	t_env_node			*head;
	t_env_node			*tail;
	ssize_t				size;
	pid_t				*pid;
}						t_env_list;

// File redirection structures
typedef struct s_file_node
{
	char				*filename;
	int					redir_type;
	int					heredoc_quoted;
	char				*heredoc_content;
	struct s_file_node	*next;
}						t_file_node;

typedef struct s_file_list
{
	t_file_node			*head;
	t_file_node			*tail;
	ssize_t				size;
}						t_file_list;

// Heredoc information
typedef struct s_heredoc_info
{
	char				*delimiter;
	int					quoted;
}						t_heredoc_info;

// Command structures
typedef struct s_cmd_node
{
	struct s_cmd_node	*next;
	int					cmd_type;
	char				**cmd;
	t_file_list			*files;
	struct s_env_list	*env;
}						t_cmd_node;

typedef struct s_cmd_list
{
	int					syntax_error;
	t_cmd_node			*head;
	t_cmd_node			*tail;
	ssize_t				size;
}						t_cmd_list;

// Includes -- END

// Structs -- BEGIN
typedef enum e_toktype
{
	TK_WORD,    // 0
	TK_BUILTIN, // 1
	S_QUOTES,   // 2 ''
	D_QUOTES,   // 3 ""
	TK_INFILE,  // 4 <
	TK_OUTFILE, // 5 >
	TK_HEREDOC, // 6 <<
	TK_APPEND,  // 7 >>
	TK_PIPE,    // 8 |
}						t_toktype;

typedef enum seg_type
{
	SEG_NO_QUOTE,
	SEG_SINGLE,
	SEG_DOUBLE,
}						t_seg_type;

typedef struct s_segment
{
	char				*value;
	t_seg_type			type;
	struct s_segment	*next;
}						t_segment;

typedef struct s_segment_list
{
	t_segment			*head;
	t_segment			*tail;
	ssize_t				size;
}						t_segment_list;

typedef struct s_token
{
	t_toktype			type;
	char				*value;
	t_segment_list		*segment_list;
	struct s_token		*next;
}						t_token;

typedef struct s_token_list
{
	int					syntax_error;
	t_token				*head;
	t_token				*tail;
	ssize_t				size;
	char				*input;
}						t_token_list;
// Structs -- END

// Lexing checking syntax
// int							main(int argc, char **argv);
bool					is_valid_red(char *str, int *i);
bool					is_red(char *str, int *i);
bool					is_valid_pipe(char *str, int *i);
bool					check_tokens(char *str, int i);
bool					is_boundary_char(char c);
bool					we_have_token(const char *str, int *i);
int						scan_quote(const char *str, int i);
bool					is_valid_quote(char *str, int *i);
int						skip_spaces(char *str, int i);
/* helpers used in token_to_cmd */
bool					is_redirection(t_toktype t);
// init all
void					init_token_lst(t_token_list *lst);
void					init_cmd_lst(t_cmd_list *lst);
void					init_env_lst(t_env_list *lst);
void					init_segment_lst(t_segment_list *lst);

// tokenize
t_token					*create_token(t_toktype type, char *val);
int						push_token(t_token_list *lst, t_token *token);
int						add_token(t_token_list *lst, t_toktype type, char *str,
							int len);
int						handle_word(t_token_list *lst, char *input, int *i);
int						tokenize(t_token_list *lst, char *input);
int						red_len(char *input, int i);
t_toktype				red_type(const char *str, int i);
int						handle_quote(char *input, int *i);
int						handle_redir(t_token_list *lst, char *input, int *i,
							int red_len);
int						word_end(char *input, int i);
// token to command
int						find_segment(t_segment_list *lst, char *str);
// static int quote_segment(t_segment_list *lst, char *str, int *i);
// static int no_quote_segment(t_segment_list *lst, char *str, int *i);
int						push_segment(t_segment_list *lst, t_segment *segment);
t_segment				*create_segment(char *start, int len, t_seg_type type);

/* Command/list helpers (implemented in parsing/command) */
t_cmd_node				*create_cmdnode(void);
void					push_cmd(t_cmd_list *lst, t_cmd_node *node);
int						collect_redirs(t_token *token, t_cmd_node *cmdnode);
bool					is_built_in(char *str);
void					create_filenode(char *filename, int red_type,
							t_file_list *filelst);

/* Environment functions */
int						get_envs(char **env, t_env_list *lst);
int						find_key(char *str, t_env_node *env);
int						find_value(char *str, t_env_node *env);
int						push_env(t_env_list *lst, t_env_node *env);
void					free_env_list(t_env_list *env);
void					remove_from_env_list(t_env_list *envlst,
							const char *key);

/* Token to command conversion */
int						token_to_cmd(t_token_list *toklst, t_cmd_list *cmdlst,
							t_env_list *envlst, int last_status);
void					final_token(t_token_list *toklst, t_env_list *envlst,
							int last_status);

/* Field splitting */
char					**split_on_spaces(char *str);
int						should_split(t_segment_list *seglst);
char					*ifs_field_split(char *str);

/* Heredoc utilities */
t_heredoc_info			*process_heredoc_delimiter(char *raw_delimiter);
char					*read_heredoc_content(char *delimiter);

/* Quote removal */
// char *remove_quotes(char *str);

/* Segment utilities used elsewhere */
void					init_segment_lst(t_segment_list *lst);
char					*segments_expand(t_segment_list *seglst,
							t_env_list *envlst, int last_status);
char					*expand_env(char *str, t_env_list *env_lst);
char					*get_expand(char *seg_str, int i, int last_status,
							t_env_list *envlst);

// // debug
// void print_tokens(const t_token_list *lst);
// void print_segment_list(const t_segment_list *list);
// void init_segment_lst(t_segment_list *lst);

#endif
