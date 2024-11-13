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

void ft_lexerclear(t_lexer **lst) {
    t_lexer *tmp;

    while (*lst) {                 // Пока список не пуст
        tmp = (*lst)->next;        // Сохраняем указатель на следующий элемент
        free((*lst)->str);         // Освобождаем память, занятую строкой элемента
        free(*lst);                // Освобождаем память самого элемента
        *lst = tmp;                // Переходим к следующему элементу
    }
    *lst = NULL;                   // Устанавливаем указатель на NULL для безопасности
}

void ft_lexerclear_one(t_lexer **lst) {
    if ((*lst)->str) {         // Проверяем, есть ли строка в элементе
        free((*lst)->str);      // Освобождаем память, занятую строкой
        (*lst)->str = NULL;     // Устанавливаем указатель строки в NULL для безопасности
    }
    free(*lst);                 // Освобождаем память, занятую самим элементом списка
    *lst = NULL;                // Устанавливаем указатель на элемент в NULL
}

void ft_lexerdelone(t_lexer **lst, int key) {
    t_lexer *node = *lst; // Начинаем с первого элемента
    t_lexer *prev = NULL; // Предыдущий элемент, инициализируем как NULL

    // Ищем элемент с заданным ключом
    while (node && node->index != key) {
        prev = node; // Запоминаем предыдущий элемент
        node = node->next; // Переходим к следующему элементу
    }

    // Если узел не найден, просто выходим
    if (!node) return;

    // Удаляем найденный элемент из списка
    if (prev) {
        prev->next = node->next; // Указываем, что предыдущий элемент теперь указывает на следующий за удаляемым
    } else {
        *lst = node->next; // Если удаляемый элемент - первый, обновляем указатель на начало списка
    }
    
    if (node->next) { // Обновляем указатель на предыдущий элемент для следующего элемента
        node->next->prev = prev;
    }

    ft_lexerclear_one(&node); // Очищаем память, занятую найденным элементом
}

int main(void)
{
    t_lexer *lexer = NULL;
    t_parser *parser = NULL;
    t_parser_tools utils = {&lexer, NULL, 0, &parser};

    // Например, инициализация лексера
    lexer = create_lexer("echo hello | grep world");  // create_lexer должна быть реализована

    parser_init(&parser, &lexer, &utils);

    // Вывод результата
    t_parser *temp = parser;
    while (temp)
    {
        printf("Command: %s\n", temp->cmd[0]);  // Пример вывода команды
        temp = temp->next;
    }

    free_parser(&parser);
    return 0;
}

void parser_part(shell **shell)
{
    parser_init(&shell->pars, &shell->lexer_list, &shell->pars_mini);
    t_lexer *tmp;
    int i;

    i = 0;
    int count_com = shell->count_pipe + 1;
    while(count_com--)
    {
        shell->pars_mini->redirection =  NULL;
        shell->pars_mini->num_redirection = 0;
        del_redir(&tmp, shell->pars_mini);
        if (shell->pars_mini)
    }
}