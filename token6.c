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


// Функция для создания нового узла в списке
t_lexer *lexer_create(char *value, t_tokens token, int index) {
    t_lexer *new = (t_lexer *)malloc(sizeof(t_lexer));
    if (!new)
        return NULL;

    new->str = value;
    new->token = token;
    new->i = index;
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

// Проверка, является ли символ пробелом или управляющим символом
int ft_ifspace(char *line) {
    return (*line == ' ' || (*line >= 9 && *line <= 13));
}

// Считывание слова до первого разделителя
int ft_read_word(char *line) {
    int i = 0;
    while (line[i] && !ft_ifspace(&line[i]) && line[i] != '|' && line[i] != '>' && line[i] != '<') {
        i++;
    }
    return i;
}

// Считывание символов внутри одинарных или двойных кавычек
int ft_read_word_quote(char *line, char quote) {
    int i = 1;
    while (line[i] && line[i] != quote) {
        i++;
    }
    return (line[i] == quote) ? i + 1 : i; // Если кавычка найдена, включаем ее в длину
}

// Функция для создания подстроки
char *substr(const char *str, size_t start, size_t len) {
    size_t str_len = strlen(str);
    if (start >= str_len) return NULL;
    if (start + len > str_len) len = str_len - start;

    char *sub = (char *)malloc(len + 1);
    if (!sub) return NULL;
    strncpy(sub, str + start, len);
    sub[len] = '\0';
    return sub;
}

// Функция для разбора строки и добавления токенов в список
t_lexer *ft_read_line(char *line, t_lexer **list) {
    int index = 0;

    while (*line) {
        while (ft_ifspace(line)) line++;

        if (*line == '|') {
            ft_create_lexer_list(NULL, T_PIPE, index, list);
        } else if (*line == '>' && *(line + 1) == '>') {
            ft_create_lexer_list(NULL, T_PLUS2, index, list);
            line++;
        } else if (*line == '>' && *(line + 1) != '>') {
            ft_create_lexer_list(NULL, T_PLUS, index, list);
        } else if (*line == '<' && *(line + 1) == '<') {
            ft_create_lexer_list(NULL, T_LESS2, index, list);
            line++;
        } else if (*line == '<' && *(line + 1) != '<') {
            ft_create_lexer_list(NULL, T_LESS, index, list);
        } else if (*line == '\'' || *line == '"') {
            char quote = *line;
            int len = ft_read_word_quote(line, quote);
            char *word = substr(line, 0, len);  // сохраняем кавычки
            ft_create_lexer_list(word, T_WORD, index, list);
            line += len - 1;
        } else {
            int len = ft_read_word(line);
            char *word = substr(line, 0, len);
            ft_create_lexer_list(word, T_WORD, index, list);
            line += len - 1;
        }
        line++;
        index++;
    }
    return *list;
}

void free_lexer_list(t_lexer **list) {
    t_lexer *current = *list;
    t_lexer *next_node;

    while (current) {
        next_node = current->next; // Сохраняем ссылку на следующий узел
        if (current->str) {
            free(current->str);    // Освобождаем память, выделенную под строку
        }
        free(current);             // Освобождаем текущий узел
        current = next_node;       // Переходим к следующему узлу
    }
    *list = NULL;                  // Обнуляем указатель на список
}


// Функция для вывода токенов (для отладки)
void print_tokens(t_lexer *list) {
    while (list) {
        printf("Index: %d, Type: %d, Value: %s\n", list->i, list->token, list->str ? list->str : "NULL");
        list = list->next;
    }
}



// Функция для обрезки строки `s1`, удаляя символы `set` с начала и конца
char *ft_strtrim(char *s1, char *set) {
    if (!s1 || !set) return NULL; // Проверка на NULL

    // Поиск первого символа, не входящего в `set`
    char *start = s1;
    while (*start && strchr(set, *start)) {
        start++;
    }

    // Поиск последнего символа, не входящего в `set`
    char *end = s1 + strlen(s1) - 1;
    while (end > start && strchr(set, *end)) {
        end--;
    }

    // Вычисляем новую длину обрезанной строки
    size_t new_length = end - start + 1;

    // Выделяем память для новой строки
    char *trimmed = malloc(new_length + 1);
    if (!trimmed) return NULL; // Проверка на успешное выделение памяти

    // Копируем обрезанную часть строки
    strncpy(trimmed, start, new_length);
    trimmed[new_length] = '\0'; // Завершаем строку

    return trimmed; // Возвращаем указатель на обрезанную строку
}

int find_matching_quote(char *line, int start, char quote) {
    int i = start + 1; // Начинаем с символа после открывающей кавычки
    while (line[i]) { // Цикл по всей строке
        if (line[i] == quote) { // Найдена закрывающая кавычка
            return i; // Возвращаем индекс закрывающей кавычки
        }
        i++;
    }
    return -1; // Если не нашли, возвращаем -1
}

int check_quotes(char *line) {
    int i = 0;
    while (line[i]) { // Цикл по всем символам строки
        if (line[i] == '"') { // Если встречаем двойную кавычку
            int closing_index = find_matching_quote(line, i, '"');
            if (closing_index == -1) { // Если не нашли закрывающую
                return 0; // Возвращаем 0 (ошибка)
            }
            i = closing_index; // Продолжаем с закрывающей кавычки
        }
        else if (line[i] == '\'') { // Если встречаем одинарную кавычку
            int closing_index = find_matching_quote(line, i, '\'');
            if (closing_index == -1) { // Если не нашли закрывающую
                return 0; // Возвращаем 0 (ошибка)
            }
            i = closing_index; // Продолжаем с закрывающей кавычки
        }
        i++; // Переходим к следующему символу
    }
    return 1; // Все кавычки корректно сопоставлены
}

void ft_init_shell(t_shell *shell)
{
    shell->input_line = NULL;
    shell->lexer_list = NULL;
}

int ft_minicheck(char *line)
{
    int i;
    int len;

    i = 0;
    len = 0;
    len = ft_strlen(line);
    while (ft_ifspace(line))
        i++;
    if (i == len)
        return(1);
    if (line[i] == '|')
    {
        printf("syntax error near unexpected token `|'");
        exit(1);
    }
     while (ft_ifspace(line[len - 1]))
        len--;
    if (line[len] == '|')
    {
        printf("syntax error near unexpected token `|'");
        exit(1);
    }
    return (0);
}

// Пример использования
int main()
{
    t_shell shell;
    char *input_line;
    while(1)
    {
        input_line = readline("Minishell$ ");
        if (input_line == NULL)
        {
            free(input_line);
            return 0; // Проверяем, что ввод не NULL
        }
        else if (ft_strlen(input_line))
        {
            add_history(input_line);
            if (ft_minicheck(input_line))
                continue;
            ft_init_shell(&shell);
            shell.input_line = input_line;
            free(input_line); // Освобождаем память после использования input_line  
            if (!check_quotes(shell.input_line))
            {
                printf("Error: unmatched quotes found.\n");
                free(shell.input_line); // Освобождаем память перед выходом
                return 1; // Выход с ошибкой
            }
           // ft_read_line(shell.input_line, &shell.lexer_list);  // Токенизация строки
           // print_tokens(shell.lexer_list); 
           // free_lexer_list(&shell.lexer_list); 
            free(shell.input_line); // Освобождаем память после использования result
        }
    }
    return 0; // Успешный выход
}
