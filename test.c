/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 14:40:48 by zserobia          #+#    #+#             */
/*   Updated: 2024/10/29 14:40:51 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

size_t ft_strlen(const char *s)
{
    size_t i = 0;
    while (s[i])
    {
        i++;
    }
    return i;
}

int find_matching_quote(char *line, int start, char quote)
{
    int i = start + 1;
    while (line[i])
    {
        if (line[i] == quote)
        {
            return i;
        }
        i++;
    }
    return (-1);
}

int ft_ifspace(char line)
{
    return (line == ' ' || (line >= 9 && line <= 13));
}

int check_quotes(char *line)
{
    int i = 0;
    while (line[i]) 
    {
        if (line[i] == '"') 
        {
            int closing_index = find_matching_quote(line, i, '"');
            if (closing_index == -1)
                return 0; 
            i = closing_index;
        }
        else if (line[i] == '\'') 
        {
            int closing_index = find_matching_quote(line, i, '\'');
            if (closing_index == -1) 
                return 0;
            i = closing_index;
        }
        i++;
    }
    return (1);
}

void ft_init_shell(t_shell *shell)
{
    shell->input_line = NULL;
    shell->lexer_list = NULL;
    shell->count_pipe = 0;
    shell->pars = NULL;
    shell->pars_mini = NULL;
}

int ft_minicheck(char *line)
{
    int i = 0;
    int len = ft_strlen(line);

    while (ft_ifspace(line[i]))
        i++;
    if (i == len)
        return 1;
    if (line[i] == '|') 
    {
        printf("-bash: syntax error near unexpected token `|'\n");
        return (1);
    }
    return (0);
}

void print_tokens(t_lexer *list)
{
    t_lexer *tmp;

    tmp = list;
    while (tmp)
    {
        printf("Index: %d, Type: %d, Value: %s\n", tmp->index, tmp->token, tmp->str);
        tmp = tmp->next;
    }
}

int ft_check_errors(t_lexer *list)
{
    t_lexer *tmp;

    tmp = list;
    while (tmp)
    {
        if (tmp->token == PIPE && tmp->next && tmp->next->token == PIPE)
        {
            printf("-bash: syntax error near unexpected token `|'\n");
            return (1);            
        }
        else if (tmp->token == PIPE && !tmp->next)
        {
            printf("-bash: syntax error near unexpected token `|'\n");
            return (1);            
        }
        else if (tmp->token > PIPE && !tmp->next)
        {
            printf("-bash: syntax error near unexpected token `newline'\n");
            return (1);            
        }
        else if (tmp->token > PIPE && tmp->next && tmp->next->token >= PIPE)    //это надо проверить
        {
            printf("-bash: syntax error near unexpected token `%s'\n", tmp->next->str);
            return (1);            
        }
        tmp = tmp->next;
    }
    return (0);
}

void free_lexer_list(t_lexer **lexer)
{
    t_lexer *current;
    t_lexer *next;

    current = *lexer;
    if (lexer == NULL || *lexer == NULL)
        return;
    while (current)
    {
        next = current->next;
        if (current->str)
            free(current->str);
        free(current);    
        current = next;
    }
    *lexer = NULL;
}

void ft_free_lex(t_shell *shell)
{
    if (shell->input_line)
    {
        free(shell->input_line);
        shell->input_line = NULL;
    }
    
    if (shell->lexer_list)
        free_lexer_list(&shell->lexer_list);
}

    void ft_free_pars(t_shell *shell)
    {
    if (shell->pars)
        free_pars(shell->pars);
    if (shell->pars_mini)
    {
        free(shell->pars_mini);
        shell->pars_mini = NULL;
    }
}

void free_pars(t_simple_cmds *pars)
{
    t_simple_cmds *current;
    t_simple_cmds *next;

    current = pars;
    while (current != NULL)
    {
        next = current->next;
        if (current->str)
        {
            for (int i = 0; current->str[i] != NULL; i++) 
            {
                free(current->str[i]);
            }
            free(current->str);
        }
        if (current->hd_file_name) {
            free(current->hd_file_name);
        }
        if (current->redirections) {
            free_lexer_list(&current->redirections);
        }
        free(current);
        current = next;
    }
    
}

void ft_start_loop()
{
    t_shell shell;

    while (1)
    {
        ft_init_shell(&shell);
        shell.input_line = readline("Minishell$ ");
        if (shell.input_line == NULL)
        {
            printf("logout\n");
            rl_clear_history();
            exit(0);
        }
        if (strlen(shell.input_line) > 0)
        {
            add_history(shell.input_line);
            if (ft_minicheck(shell.input_line))
            {
               ft_free_lex(&shell);
                continue;
            }
            if (!check_quotes(shell.input_line))
            {
                printf("-bash: Error: unmatched quotes found.\n");
                ft_free_lex(&shell);
                continue;
            }
            ft_read_token(shell.input_line, &shell.lexer_list, &shell);
            print_tokens(shell.lexer_list); 
            if (ft_check_errors(shell.lexer_list))
            {
                ft_free_lex(&shell);
                continue;
            }
            parser_part(shell.count_pipe, shell.lexer_list);
           //ft_free_lex(&shell);
           //ft_free_pars(&shell);
            if (shell.input_line)
            {
                free(shell.input_line);
                shell.input_line = NULL;
            }
        }
        else
            ft_free_lex(&shell); // Освобождаем память при пустом вводе
    }
}

int main()
{
    ft_start_loop(); 
    rl_clear_history();
    return 0;
}