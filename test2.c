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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <stdlib.h>

// Define other necessary structures and functions

#include <stdlib.h>
#include <string.h>

// Duplicate a string safely with malloc
char *ft_strdup(const char *s) {
    size_t i = 0;
    char *str = malloc(sizeof(char) * (strlen(s) + 1));
    if (!str) return NULL;
    while (s[i]) {
        str[i] = s[i];
        i++;
    }
    str[i] = '\0';
    return str;
}

// Deletes a node from the lexer list
/*void ft_lexerdelone(t_lexer **list, int index) {
    if (!list || !*list) return;
    
    t_lexer *current = *list;
    t_lexer *previous = NULL;
    while (current) {
        if (current->index == index) {
            if (!previous) {
                *list = current->next;
                if (current->next) current->next->prev = NULL;
            } else {
                previous->next = current->next;
                if (current->next) current->next->prev = previous;
            }
            free(current->str);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}*/
void ft_lexerclear_one(t_lexer **lst) {
    if ((*lst)->str) {         // Проверяем, есть ли строка в элементе
        free((*lst)->str);      // Освобождаем память, занятую строкой
        (*lst)->str = NULL;     // Устанавливаем указатель строки в NULL для безопасности
    }
    free(*lst);                 // Освобождаем память, занятую самим элементом списка
    *lst = NULL;                // Устанавливаем указатель на элемент в NULL
}

void ft_lexerdelone(t_lexer **list, int index) {
    t_lexer *node = *list; // Начинаем с первого элемента
    t_lexer *prev = NULL; // Предыдущий элемент, инициализируем как NULL
    t_lexer *next;
    // Ищем элемент с заданным ключом
    while (node && node->index != index) {
        node = node->next;
        if (!node) return;
        prev = node-prev; // Запоминаем предыдущий элемент
        next = node->next; // Переходим к следующему элементу
    }
    // Удаляем найденный элемент из списка
    if (prev == NULL) {
        free(node);
        node = NULL;
        *list = next;
        if (next)
            (*list)->prev = NULL;
        return ;
    }
    node->prev->next = next; // Указываем, что предыдущий элемент теперь указывает на следующий за удаляемым
    if (node->next)
        node->next->prev = prev;
    free(node);
    node = NULL;
}
    
    if (node->next) { // Обновляем указатель на предыдущий элемент для следующего элемента
        node->next->prev = prev;
    }

    ft_lexerclear_one(&node); // Очищаем память, занятую найденным элементом
}

// Frees the parser list along with its contents
void free_parser_list(t_simple_cmds **list) {
    t_simple_cmds *current = *list;
    while (current) {
        t_simple_cmds *next = current->next;
        
        if (current->str) {
            for (int i = 0; current->str[i]; i++) {
                free(current->str[i]);
            }
            free(current->str);
        }

        free(current->hd_file_name);

        if (current->redirections) {
            free_redirections(current->redirections);
        }

        free(current);
        current = next;
    }
    *list = NULL;
}


// Creates a parser node
t_simple_cmds *ft_parser_create(char **value, int num_redirections, t_lexer *redirection) {
    t_simple_cmds *new = (t_simple_cmds *)malloc(sizeof(t_simple_cmds));
    if (!new) return NULL;

    new->str = value;
    new->num_redirections = num_redirections;
    new->hd_file_name = NULL;
    new->redirections = redirection;
    new->next = NULL;
    new->prev = NULL;
    return new;
}

// Adds a parser node to the end of the list
void parser_add_back(t_simple_cmds **list, t_simple_cmds *new) {
    if (!*list) {
        *list = new;
        return;
    }
    t_simple_cmds *temp = *list;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = new;
    new->prev = temp;
}

// Wrapper for adding a new parser node
void ft_create_parser_list(char **value, int num_redirections, t_lexer *redirection, t_simple_cmds **list) {
    t_simple_cmds *new_parser = ft_parser_create(value, num_redirections, redirection);
    if (new_parser) {
        parser_add_back(list, new_parser);
    }
}

// Counts the tokens in a lexer list
int count_token(t_lexer *new) {
    int arg_count = 0;
    t_lexer *tmp = new;
    while (tmp) {
        if (tmp->token > PIPE) {
            if (tmp->next && tmp->next->next) {
                tmp = tmp->next->next;
            } else {
                tmp = tmp->next;
                break;
            }
        }else if (tmp->token == WORD){
            arg_count++;
            tmp = tmp->next;
        }
    }
    return arg_count;
}

// Frees all redirections in the lexer list
void free_redirections(t_lexer *redirection) {
    t_lexer *current = redirection;
    while (current) {
        t_lexer *next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

t_simple_cmds *init_cmd(t_parser_tools *parser_tools)
{
    char **str;
    int i;
    int arg_size;
    t_lexer *tmp;

    i = 0;
    rm_redir(parser_tools);
    arg_size = count_token(parser_tools->lexer_list);
    str = ft_calloc(arg_size + 1, sizeof(char *));
    if (!str)
    {
        free_lexer_list(parser_tools->lexer_list);
        printf("error");
        break ;
    }
    	tmp = parser_tools->lexer_list; // Временный указатель для обхода списка лексера
	   while (arg_size > 0) // Цикл, пока есть аргументы
	  {
		if (tmp->str) // Проверка, что текущий элемент содержит строку
		{
			str[i++] = ft_strdup(tmp->str); // Копируем строку аргумента в массив
			ft_lexerdelone(&parser_tools->lexer_list, tmp->i); // Удаляем текущий элемент из списка лексера
			tmp = parser_tools->lexer_list; // Обновляем указатель для продолжения обхода
		}
		arg_size--; // Уменьшаем количество оставшихся аргументов
	}
	return (ft_simple_cmdsnew(str, parser_tools->num_redirections, parser_tools->redirections)); // Создаем команду с аргументами и перенаправлениями
}

t_parser_tools init_parser_tools(t_lexer *lexer_list, t_tools *tools)
{
    t_parser_tools parser_tools;

    parser_tools.lexer_list = lexer_list;
    parser_tools.redirections = NULL;
    parser_tools.str = NULL;
    parser_tools.num_redirections = 0;
    parser_tools.shell = tools;
    return (parser_tools);
}
// Core parser function
void parser_part(t_shell *shell) {

    t_simple_cmds *node;
    t_parser_tools parser_tools;

    while (shell->lexer_list) {
        parser_tools = init_parser_tools(shell->lexer_list, shell);
        node = init_cmd(shell->lexer_list, shell);


        int arg_count = count_token(current);
        command_args = malloc(sizeof(char *) * (arg_count + 1));
        if (!command_args) return;

        // Loop through tokens until a PIPE is encountered
        while (current && current->token != PIPE) {
            if (current->token > PIPE) {  // Handle redirection tokens
                ft_create_lexer_list(current->next->str, current->token, current->index, &redirection);
                num_redirections++;

                int ind1 = current->index;
                current = current->next;  // Move to next token (redirection target)
                
                if (current) {
                    int ind2 = current->index;
                    current = current->next;  // Move to next token after redirection target
                    ft_lexerdelone(list, ind1);  // Delete redirection operator node
                    ft_lexerdelone(list, ind2);  // Delete redirection target node
                }
            } else {  // Handle command arguments
                command_args[i++] = ft_strdup(current->str);
                free(current->str);
                current = current->next;
            }
        }

        printf("redir str %s  token  %d\n", redirection->str, redirection->token);
        printf("redir str %s  token  %d\n", redirection->next->str, redirection->next->token);
}}
        /*

        // Create and add the command node to the list
        t_simple_cmds *new_cmd = ft_parser_create(command_args, num_redirections, redirection);
        if (new_cmd) {
            parser_add_back(list1, new_cmd);
        }

        // Handle PIPE token, if present, and move current to the next command
        if (current && current->token == PIPE) {
            int pipe_index = current->index;
            current = current->next;
            ft_lexerdelone(list, pipe_index);  // Delete PIPE token node
        }

        // Free redirection nodes, if any
        if (redirection) {
            free_redirections(redirection);
            redirection = NULL;
        }

        // Free each command argument string and the array itself
        if (command_args) {
            for (int j = 0; j < i; j++) {
                free(command_args[j]);
            }
            free(command_args);
            command_args = NULL;
        }
    }
}*/




/*
// Обновленная версия parser_part_2
void parser_part_2(t_lexer **redirection, t_lexer *new, t_simple_cmds **list1, int *num_redirections, char **command_args)
{
    int i = 0;
    while (new && new->token != PIPE) {
        if (new->token > PIPE) {  // Это редирект
            // Добавляем редирект в список редиректов
            ft_create_lexer_list(new->str, new->token, new->index, redirection);
            (*num_redirections)++;

            int ind1 = new->index;
            new = new->next;  // Переходим к аргументу редиректа
            int ind2 = new ? new->index : -1;
            new = new->next;  // Переходим к следующему токену

            // Удаляем редирект и его аргумент
            ft_lexerdelone(&new, ind1);
            if (ind2 != -1) ft_lexerdelone(&new, ind2);
        } else {  // Обычный аргумент
            command_args[i++] = ft_strdup(new->str);
            new = new->next;
        }
    }
    command_args[i] = NULL;  // Завершаем список аргументов

    // Создаем команду с аргументами и редиректами и добавляем в список
    t_simple_cmds *new_cmd = ft_parser_create(command_args, *num_redirections, *redirection);
    if (new_cmd) {
        parser_add_back(list1, new_cmd);
    }
}
*/


/*
void print_parser_list(t_simple_cmds *list) {
    t_simple_cmds *current = list;
    while (current) {
        // Print the arguments for the current command
        if (current->str) {
            for (int i = 0; current->str[i]; i++) {
                printf("Arg[%d]: %s\n", i, current->str[i]);
            }
        }

        // Print redirections
        t_lexer *redir = current->redirections;
        while (redir) {
            printf("Redirection: %s\n", redir->str);
            redir = redir->next;
        }

        current = current->next;
    }
}*/
