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

char	*ft_strdup(const char *s)
{
	size_t	i;
	char	*str;

	i = 0;
	str = malloc(sizeof(char) * ft_strlen(s) + 1);
	if (!str)
		return (NULL);
	while (s[i])
	{
		str[i] = ((char *)s)[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}


void ft_lexerdelone(t_lexer **list, int index) {
    if (!list || !*list) {
        return;  // Ensure the list is not NULL
    }

    t_lexer *current = *list;
    t_lexer *previous = NULL;

    // Поиск узла с нужным индексом
    while (current) {
        if (current->index == index) {
            // Если элемент - первый в списке
            if (!previous) {
                *list = current->next;
                if (current->next) {
                    current->next->prev = NULL;
                }
            } else { 
                // Устанавливаем связи для предыдущего и следующего элементов
                previous->next = current->next;
                if (current->next) {
                    current->next->prev = previous;
                }
            }

            // Освобождаем память, выделенную для строки и узла
            if (current->str) {
                free(current->str);
            }
            free(current);
            return;  // Exit after deleting the node
        }

        // Переходим к следующему узлу
        previous = current;
        current = current->next;
    }
}



void free_parser_list(t_simple_cmds **list) {
    t_simple_cmds *current = *list;
    t_simple_cmds *next;

    while (current) {
        next = current->next; // Сохраняем ссылку на следующий узел
        
        // Освобождаем массив строк str
        if (current->str) {
            for (int i = 0; current->str[i]; i++) {
                free(current->str[i]);  // Освобождаем каждую строку в массиве
            }
            free(current->str);         // Освобождаем сам массив
        }
        
        // Освобождаем hd_file_name, если оно было выделено
        if (current->hd_file_name) {
            free(current->hd_file_name);
        }

        // Освобождаем список redirections
        if (current->redirections) {
            free_lexer_list(&current->redirections);
        }

        free(current); // Освобождаем текущий узел структуры t_simple_cmds
        current = next; // Переходим к следующему узлу
    }
    
    *list = NULL; // Обнуляем указатель на список
}

t_simple_cmds *ft_parser_create(char **value, int num_redirections, t_lexer *redirection) {
    t_simple_cmds *new = (t_simple_cmds *)malloc(sizeof(t_simple_cmds));
    if (!new)
        return NULL;

    new->str = value;
    //new->builtin = choose_exec[str[0]];
    new->num_redirections = num_redirections;
    new->hd_file_name = NULL;
    new->redirections = NULL;
    new->next = NULL;
    new->prev = NULL;
    return (new);
}

// Функция для добавления узла в конец списка
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

// Функция для создания и добавления токена в список
void ft_create_parser_list(char **value, int num_redirections, t_lexer *redirection, t_simple_cmds **list) {
    t_simple_cmds *new_parser = ft_parser_create(value, num_redirections, redirection);
    if (new_parser) {
        parser_add_back(list, new_parser);
    }
}

int count_token(t_lexer *new) {
    int arg_count = 0;
    t_lexer *tmp = new;
    while (tmp) {
        if (tmp->token > PIPE) {  // It's a redirection
            if (tmp->next && tmp->next->next) {
                tmp = tmp->next->next;  // Skip redirection and its argument
            } else {
                tmp = tmp->next;
                break;  // Handle case where redirection argument is missing
            }
        } else {
            arg_count++;  // Regular argument
            tmp = tmp->next;
        }
    }
    return arg_count;
}


// Helper function to free the redirection list for a command
void free_redirections(t_lexer *redirection) {
    t_lexer *current = redirection;
    while (current) {
        t_lexer *next = current->next;
        free(current->str);  // Free string if dynamically allocated
        free(current);       // Free the redirection node
        current = next;
    }
}

// Declare parser_part_2 before using it
void parser_part_2(t_lexer **redirection, t_lexer *new, t_simple_cmds **list1, int *num_redirections, char **command_args);
void parser_part(t_lexer **list, t_simple_cmds **list1) {
    t_lexer *new = *list;
    char **command_args = NULL;

    while (new) {
        int i = 0;
        int num_redirections = 0;
        t_lexer *redirection = NULL;

        // Подсчитываем количество токенов, включая редиректы
        int arg_count = count_token(new);
        command_args = malloc(sizeof(char *) * (arg_count + 1));  // +1 для NULL завершения
        if (!command_args) {
            return;  // Ошибка выделения памяти
        }

        // Обрабатываем лексемы до PIPE или конца списка
        t_lexer *current = new;
        while (current && current->token != PIPE) {
            // Передаем num_redirections, command_args и i в parser_part_2
            parser_part_2(&redirection, current, list1, &num_redirections, command_args);

            // Если это редирект, пропускаем токен, который мы уже обработали
            if (current->token > PIPE) {
                // Пропускаем токен редиректа и его аргумент
                int ind1 = current->index;
                current = current->next;  // Переходим к аргументу редиректа
                int ind2 = current ? current->index : -1;
                current = current->next;  // Переходим к следующему токену

                // Удаляем редирект и его аргумент
                ft_lexerdelone(&new, ind1);
                if (ind2 != -1) ft_lexerdelone(&new, ind2);
            } else {
                current = current->next;  // Идем к следующей лексеме
            }
        }

        // После цикла мы либо остановились на PIPE, либо достигли конца
        // Если встретили PIPE, удаляем его
        if (current && current->token == PIPE) {
            int pipe_index = current->index;
            new = current->next;  // Перемещаемся на следующее значение после PIPE
            ft_lexerdelone(&new, pipe_index);  // Удаляем PIPE
        } else {
            new = current;  // Если PIPE не было, выходим
        }

        // Освобождаем память для аргументов
        for (int j = 0; j < i; j++) {
            free(command_args[j]);
        }
        free(command_args);
        command_args = NULL;

        // Освобождаем память для редиректов
        free_lexer_list(&redirection);
    }
}

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
}



t_lexer *lexer_create(char *value, t_tokens token, int index) {
    t_lexer *new = (t_lexer *)malloc(sizeof(t_lexer));
    if (!new)
        return NULL;

    new->str = value;
    new->token = token;
    new->index = index;
    new->next = NULL;
    new->prev = NULL;

    return new;
}

// Функция для добавления узла в конец списка
void lexer_add_back(t_lexer **list, t_lexer *new_token) {
    if (!*list) {
        *list = new_token;
        return;
    }
    t_lexer *temp = *list;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = new_token;
    new_token->prev = temp;
}

// Функция для создания и добавления токена в список
void ft_create_lexer_list(char *value, t_tokens type, int index, t_lexer **list) {
    t_lexer *new_token = lexer_create(value, type, index);
    if (new_token) {
        lexer_add_back(list, new_token);
    }
}

void	parser_init(t_parser **parser, t_lexer **lexer, t_parser_utils *utils)
{
	t_lexer	*lxr;
	int		i;
	int		cmds;

	i = 0;
	lxr = *lexer;                                 // Начинаем с первого узла лексера
	cmds = count_pipes(lxr) + 1;                  // Подсчитываем количество команд (по количеству пайпов + 1)
	while (cmds--)                                // Цикл по количеству команд
	{
		utils->redirections = NULL;               // Обнуляем редиректы для текущей команды
		utils->nb_of_redirections = 0;            // Обнуляем счетчик редиректов
		detach_redirections(&lxr, utils);         // Отсоединяем редиректы от команды
		if (*utils->lexer == NULL)                // Если больше нет команд
			return ;                              // Завершаем выполнение
		if (!i++)                                 // Если это первая команда
			*lexer = lxr;                         // Устанавливаем начальный лексер
		else
			lxr = lxr->next;                      // Иначе переходим к следующему элементу
		add_node_parser(parser, new_node_parser(lxr, utils)); // Создаем и добавляем узел парсера
		if (cmds == 0)                            // Если больше команд нет
			break ;                               // Выходим из цикла
		while (lxr->token != PIPE)                // Переходим к следующей команде, пока не встретится пайп
			lxr = lxr->next;
	}
	free_lexer(lexer);                            // Освобождаем память, занятую лексером
}








,0

int main() {
    // Создаем массив для лексера с помощью malloc
    t_lexer *lexer = malloc(sizeof(t_lexer) * 7);  // 7 элементов

    // Инициализируем элементы лексера
    lexer[0] = (t_lexer){0, "cmd1", 0, &lexer[1], NULL};
    lexer[1] = (t_lexer){OUTPUT, ">", 1, &lexer[2], &lexer[0]};
    lexer[2] = (t_lexer){0, "file.txt", 2, &lexer[3], &lexer[1]};
    lexer[3] = (t_lexer){PIPE, "|", 3, &lexer[4], &lexer[2]};
    lexer[4] = (t_lexer){0, "cmd2", 4, &lexer[5], &lexer[3]};
    lexer[5] = (t_lexer){APPENDOUTPUT, ">>", 5, &lexer[6], &lexer[4]};
    lexer[6] = (t_lexer){0, "file2.txt", 6, NULL, &lexer[5]};

    // Устанавливаем указатель на начало списка
    t_lexer *lexer_ptr = lexer;

    // Обрабатываем редиректы
    detach_redirections(&lexer_ptr);

    // Освобождаем массив лексеров (но не их содержимое)
    free(lexer);  // Освобождаем только массив структур t_lexer

    return 0;
}
