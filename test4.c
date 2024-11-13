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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_lexer t_lexer;
typedef struct s_parser t_parser;
typedef struct s_parser_utils t_parser_utils;

typedef enum token
{
    WORD,
    PIPE,
    INPUT,
    OUTPUT,
    HEREDOC,
    APPENDOUTPUT
} t_token;

typedef struct s_lexer
{
    char *content;
    t_token token;
    int index;
    struct s_lexer *next;
    struct s_lexer *prev;
} t_lexer;

typedef struct s_parser
{
    char **cmd;
    int (*builtin)(t_env **, struct s_parser *);
    t_lexer *redirections;
    int nb_of_redirections;
    int pid;
    struct s_parser *next;
    struct s_parser *prev;
} t_parser;

typedef struct s_parser_utils
{
    t_lexer **lexer;
    t_lexer *redirections;
    int nb_of_redirections;
    struct s_parser **parser;
} t_parser_utils;


void ft_error(char *error, t_parser_utils *utils);
void free_lexer(t_lexer **lexer);
void free_parser(t_parser **parser);
void rm_from_lexer(t_lexer **lexer, int index);
t_parser *new_node_parser(t_lexer *lexer, t_parser_utils *utils);
void add_node_parser(t_parser **lst, t_parser *newnode);
void parser_init(t_parser **parser, t_lexer **lexer, t_parser_utils *utils);
void detach_redirections(t_lexer **lexer, t_parser_utils *utils);
void add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_parser_utils *utils);
void token_error(t_lexer *problem, t_parser_utils *utils);

static t_parser *new_node_parser(t_lexer *lexer, t_parser_utils *utils)
{
    t_parser *new = malloc(sizeof(t_parser));
    if (!new)
        return NULL;

    new->redirections = utils->redirections;
    new->nb_of_redirections = utils->nb_of_redirections;
    new->cmd = build_command(lexer);  // Функция build_command должна быть реализована
   // new->builtin = is_builtin(new->cmd);  // Функция is_builtin должна быть реализована
    new->pid = 0;
    new->next = NULL;
    new->prev = NULL;
    return new;
}

char	**build_command(t_lexer *lexer)
{
	char	**cmd;
	int		i;
	int		len;

	i = -1;                               // Устанавливаем индекс i в -1 для удобства инкрементации в цикле
	if (!lexer)                           // Если список пуст, возвращаем NULL
		return (NULL);                    // Нет узлов для обработки
	len = count_nodes(lexer);             // Подсчитываем количество узлов, составляющих команду
	cmd = malloc(sizeof(char *) * (len + 1)); // Выделяем память для массива строк длиной len + 1
	while (++i < len)                     // Заполняем массив строками команды до PIPE или конца списка
	{
		cmd[i] = ft_strdup(lexer->content); // Копируем содержимое узла в cmd[i]
		lexer = lexer->next;               // Переходим к следующему узлу
	}
	cmd[i] = NULL;                        // Завершаем массив, добавляя NULL в конец
	return (cmd);                         // Возвращаем массив строк, представляющий команду
}


static void add_node_parser(t_parser **lst, t_parser *newnode)
{
    t_parser *last;
    if (*lst)
    {
        last = *lst;
        while (last->next != NULL)
            last = last->next;
        last->next = newnode;
        newnode->prev = last;
    }
    else
    {
        *lst = newnode;
    }
}

int	count_pipes(t_lexer *lexer)
{
	int	i;

	i = 0;                               // Инициализируем счетчик PIPE в 0
	while (lexer)                        // Проходим по всем узлам
	{
		if (lexer->token == PIPE)        // Если токен текущего узла — PIPE
			++i;                         // Увеличиваем счетчик PIPE
		lexer = lexer->next;             // Переходим к следующему узлу
	}
	return (i);                          // Возвращаем общее количество PIPE в списке
}


void parser_init(t_parser **parser, t_lexer **lexer, t_parser_utils *utils)
{
    t_lexer *lxr = *lexer;
    int cmds = count_pipes(lxr) + 1;
    int i = 0;

    while (cmds--)
    {
        utils->redirections = NULL;
        utils->nb_of_redirections = 0;
        detach_redirections(&lxr, utils);
        if (*utils->lexer == NULL)
            return;

        if (!i++)
            *lexer = lxr;
        else
            lxr = lxr->next;

        add_node_parser(parser, new_node_parser(lxr, utils));
        if (cmds == 0)
            break;

        while (lxr->token != PIPE)
            lxr = lxr->next;
    }
    free_lexer(lexer);
}

void ft_error(char *error, t_parser_utils *utils)
{
    printf("%s\n", error);
    free_lexer(utils->lexer);
    free_parser(utils->parser);
}

void rm_from_lexer(t_lexer **lexer, int index)
{
    t_lexer *node = *lexer;
    t_lexer *prev;
    t_lexer *next;

    while (node && node->index != index)
        node = node->next;
    if (!node)
        return;

    prev = node->prev;
    next = node->next;

    if (prev == NULL)
    {
        free(node);
        *lexer = next;
        if (next)
            next->prev = NULL;
        return;
    }

    node->prev->next = next;
    if (node->next)
        node->next->prev = prev;
    free(node);
}

t_lexer	*new_node_lexer(char *content, t_token tk, int i)
{
	t_lexer	*new;

	new = malloc(sizeof(t_lexer));
	if (!new)
		return (0);
	new->token = tk;
	new->content = content;
	new->index = i;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

void	add_node_lexer(t_lexer **lst, t_lexer *newnode)
{
	t_lexer	*last;

	if (*lst)
	{
		last = last_node(*lst);
		last->next = newnode;
		newnode->prev = last;
	}
	else
		*lst = newnode;
}

void add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_parser_utils *utils)
{
    t_lexer *newnode;
    int i1, i2;

    newnode = new_node_lexer(to_add->next->content, to_add->token, utils->nb_of_redirections);
    if (!newnode)
        return ft_error("Memory allocation error", utils);

    add_node_lexer(&utils->redirections, newnode);
    i1 = to_add->index;
    i2 = to_add->next->index;
    rm_from_lexer(lexer, i1);
    rm_from_lexer(lexer, i2);
    ++utils->nb_of_redirections;
}

void token_error(t_lexer *problem, t_parser_utils *utils)
{
    if (!problem)
        printf("Syntax error: `newline'\n");
    else
    {
        if (problem->token == INPUT)
            printf("Syntax error: `<'\n");
        else if (problem->token == OUTPUT)
            printf("Syntax error: `>'\n");
        else if (problem->token == HEREDOC)
            printf("Syntax error: `<<'\n");
        else if (problem->token == APPENDOUTPUT)
            printf("Syntax error: `>>'\n");
        else
            printf("Syntax error: `|'\n");
    }

    free_lexer(utils->lexer);
    free_parser(utils->parser);
}

void detach_redirections(t_lexer **lexer, t_parser_utils *utils)
{
    t_lexer *to_remove;

    if ((*lexer)->token == PIPE)
        to_remove = (*lexer)->next;
    else
        to_remove = *lexer;

    while (to_remove && to_remove->token == 0)
        to_remove = to_remove->next;

    if (!to_remove || (to_remove->token == PIPE && to_remove->next && to_remove->next->token != PIPE))
        return;

    if (!to_remove->next || to_remove->next->token)
        return token_error(to_remove->next, utils);

    add_new_redirection(to_remove, lexer, utils);
    if (!(*lexer))
        return;

    detach_redirections(lexer, utils);
}

void free_parser(t_parser **parser)
{
    t_parser *temp;
    while (*parser)
    {
        temp = (*parser)->next;
        if ((*parser)->cmd)
            free_tab((*parser)->cmd);  // Функция free_tab должна быть реализована
        free_lexer(&(*parser)->redirections);
        free(*parser);
        *parser = temp;
    }
    *parser = NULL;
}

int main(void)
{
    t_lexer *lexer = NULL;
    t_parser *parser = NULL;
    t_parser_utils utils = {&lexer, NULL, 0, &parser};

    t_lexer *lexer = malloc(sizeof(t_lexer) * 7);  // 7 элементов

    // Инициализируем элементы лексера
    lexer[0] = (t_lexer){0, "cmd1", 0, &lexer[1], NULL};
    lexer[1] = (t_lexer){OUTPUT, ">", 1, &lexer[2], &lexer[0]};
    lexer[2] = (t_lexer){0, "file.txt", 2, &lexer[3], &lexer[1]};
    lexer[3] = (t_lexer){PIPE, "|", 3, &lexer[4], &lexer[2]};
    lexer[4] = (t_lexer){0, "cmd2", 4, &lexer[5], &lexer[3]};
    lexer[5] = (t_lexer){APPENDOUTPUT, ">>", 5, &lexer[6], &lexer[4]};
    lexer[6] = (t_lexer){0, "file2.txt", 6, NULL, &lexer[5]};

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
