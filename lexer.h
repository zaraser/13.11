/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mobonill <mobonill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 18:05:33 by mobonill          #+#    #+#             */
/*   Updated: 2024/11/02 17:54:13 by mobonill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// TO COMPILE : gcc lexer.c -lreadline


#ifndef LEXER_H
# define LEXER_H

//# include "../libft/libft.h"
# include <limits.h>
# include <fcntl.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <sys/wait.h>
# include <errno.h>
# include <pthread.h>
# include <sys/time.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>

#include <ctype.h>

typedef enum {
    WORD,
    PIPE,
    IN,    //<
    HEREDOC, // <<
    OUT, //>
    APPEND, //>>
} t_tokens;


typedef struct s_lexer {
    char *str;
    t_tokens token;
    int index;
    struct s_lexer *next;
    struct s_lexer *prev;
} t_lexer;

typedef struct s_simple_cmds  // Use the struct name as s_simple_cmds
{
    char                    **str;               // Command arguments
    int                     num_redirections;    // Number of redirections
    char                    *hd_file_name;        // Here document file name (if applicable)
    t_lexer                 *redirections;        // List of redirections
    struct s_simple_cmds    *next;                // Pointer to next command node
    struct s_simple_cmds    *prev;                // Pointer to previous command node
} t_simple_cmds;               // Typedef name


typedef struct s_pars_mini
{
	t_lexer *lexer_list;
	t_lexer *redirections;
	int num_redirections;
	char **str;
	struct s_shell *shell;
} t_pars_mini;

typedef struct s_shell
{
    char *input_line;
	int count_pipe;
    t_lexer *lexer_list;
	t_simple_cmds *pars;
	t_pars_mini *pars_mini;



}t_shell;


void ft_start_loop();
int ft_minicheck(char *line);
void ft_init_shell(t_shell *shell);
int check_quotes(char *line);
int ft_ifspace(char line);
int find_matching_quote(char *line, int start, char quote);
size_t ft_strlen(const char *s);
int ft_read_word(char *line);
int ft_read_word_quote(char *line, char quote);
char *ft_substr(const char *str, size_t start, size_t len);
int ft_read_token_1(char *line, t_lexer **list, int index);
void ft_read_token(char *line, t_lexer **list, t_shell *shell);
void ft_create_lexer_list(char *value, t_tokens type, int index, t_lexer **list);
void lexer_add_back(t_lexer **list, t_lexer *new_token);
t_lexer *lexer_create(char *value, t_tokens token, int index);
void print_tokens(t_lexer *list);
void free_lexer_list(t_lexer **lexer);
void ft_lexerdelone(t_lexer **list, int index);
void free_parser_list(t_simple_cmds **list);
void parser_add_back(t_simple_cmds **list, t_simple_cmds *new);
void ft_create_parser_list(char **value, int num_redirection, t_lexer *redirection, t_simple_cmds **list);
//void parser_part(t_shell *shell);
void parser_part(int count_pipe, t_lexer *lexer_list);
void print_parser_list(t_simple_cmds *parser_list);
t_simple_cmds *ft_parser_create(char **value, int num_redirection, t_lexer *redirection);
char	*ft_strdup(const char *s);
void free_redirections(t_lexer *redirection);

void free_pars(t_simple_cmds *pars);
void ft_free(t_shell *shell);
void free_pars(t_simple_cmds *pars);
void ft_free_pars(t_shell *shell);
void ft_free_lex(t_shell *shell);
int add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_pars_mini *utils);
//void parser_part(int count_pipe, t_lexer *lexer_list, t_simple_cmds *commands);

#endif

/*
typedef enum token
{
	WORD,
	PIPE,
	SIMPLEQUOTE,
	DOUBLEQUOTE,
	IN_REDIR, // < 
	OUT_REDIR, // >
	DELIMITER, // <<
	APPEND, // >>
	HERE_DOC,
	SPACES,
}	token;

typedef struct s_lexer
{
	char	*str;
	int		token;

}	t_lexer;


typedef struct s_exec
{
	int	exit_status;
	
}	t_exec;

typedef struct s_env
{
	char			*name;
	char			*value;
	char			*content;
	int				index;
	struct s_env	*next;
}					t_env;

// ENVP
void	init_envp(const char **envp);
void	get_env_names_and_values(t_env *env);
void	ft_envclear(t_env **env, void (*del)(void *));
t_env	*ft_envnew(char *content);
t_env	*ft_envlast(t_env *env);
void	ft_env_add_back(t_env **env, t_env *new);
void	print_env(t_env *env);
void	free_env(t_env *env);

// LEXER
void	minishell_loop();


#endif*/