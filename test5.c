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
/*
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
}*/

/*void	ft_error(char *error, t_pars_mini *utils)
{
	printf("%s\n", error);                // Печатает сообщение об ошибке
	free_lexer_list(utils->lexer_list);             // Освобождает память, связанную с лексером
	//free_parser(utils->parser);           // Освобождает память, связанную с парсером
}*/


char *ft_strdup(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);  // Allocate memory for the string + null terminator
    if (!copy) {
        return NULL;  // If malloc fails, return NULL
    }
    memcpy(copy, s, len + 1);  // Copy the string into the newly allocated memory
    return copy;
}


void rm_from_lexer(t_lexer **lexer, int index) {
    t_lexer *node = *lexer;
    t_lexer *prev = NULL;

    // Search for the node with the specified index
    while (node && node->index != index) {
        prev = node;
        node = node->next;
    }

    // If the node is not found, exit the function
    if (!node) {
        return;
    }

    // Reassign pointers to remove the node from the list
    if (prev) {
        prev->next = node->next;
    } else {
        *lexer = node->next;
    }

    if (node->next) {
        node->next->prev = prev;
    }

    // Free the string associated with the node, if it exists
    if (node->str) {
        free(node->str);
        node->str = NULL;  // Nullify the pointer after freeing
    }

    // Free the node itself, and ensure that no further access to this node occurs
    free(node);
    node = NULL;  // Nullify to avoid any accidental usage later
}


int add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_pars_mini *utils) {
    t_lexer *newnode;
    int i1, i2;

    // Проверяем, что to_add и его следующий элемент не NULL
    if (!to_add || !to_add->next) {
        printf("Error: to_add or to_add->next is NULL\n");
        return 1;
    }

    // Создаем новый узел для редиректа
    newnode = lexer_create(ft_strdup(to_add->next->str), to_add->token, utils->num_redirections);  
    if (!newnode) {
        printf("Error: malloc failed\n");
        return 1;
    }

    // Добавляем новый редирект в список редиректов
    lexer_add_back(&utils->redirections, newnode);    

    // Сохраняем индексы для удаления лексем
    i1 = to_add->index;
    i2 = to_add->next->index;

    // Удаляем текущий и следующий узлы из лексера
    rm_from_lexer(lexer, i1);                        
    rm_from_lexer(lexer, i2);

    // Увеличиваем счетчик редиректов
    ++utils->num_redirections;                     

    return 0;
}



/*void add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_pars_mini *utils) {
    t_lexer *newnode;

    if (!to_add || !to_add->next) {
        return;
    }

    newnode = lexer_create(ft_strdup(to_add->next->str), to_add->token, -1);
    if (!newnode) {
        printf("malloc error");
        return;
    }

    lexer_add_back(&utils->redirections, newnode);
    rm_from_lexer(lexer, to_add->index);
    rm_from_lexer(lexer, to_add->next->index);

    ++utils->num_redirections;
}*/


/*void detach_redirections(t_lexer **lexer, t_pars_mini *utils) {
    t_lexer *tmp;

    while (*lexer) {
        tmp = *lexer;
        while (tmp && tmp->token == 0)
            tmp = tmp->next;

        if (tmp && tmp->token > PIPE) {
            add_new_redirection(tmp, lexer, utils);
        } else {
            break;
        }
    }
}*/

int	detach_redirections(t_lexer **lexer, t_pars_mini *utils)
{
	t_lexer	*to_remove;

    to_remove = *lexer;
	while (to_remove && to_remove->token == 0)  // Ищем редирект
		to_remove = to_remove->next;
    if (to_remove->token > 1)
    {
	    if (add_new_redirection(to_remove, lexer, utils))
            return 1;  // Добавляем новый редирект
    }
	if (!(*lexer))  // Если лексер пуст, выходим
		return 1;
	detach_redirections(lexer, utils);  // Рекурсивно продолжаем обработку редиректов
    return 0;
}


int count_nodes(t_lexer *lexer) {
    int res = 0;
    while (lexer && lexer->token != PIPE) {
        ++res;
        lexer = lexer->next;
    }
    return res;
}

char **build_command(t_lexer *lexer) {
    int len = count_nodes(lexer);
    char **cmd = malloc(sizeof(char *) * (len + 1));
    int i = 0;

    if (!cmd)
        return NULL;

    while (i < len && lexer) {
        cmd[i] = ft_strdup(lexer->str);
        if (!cmd[i]) {
            while (i-- > 0) free(cmd[i]);
            free(cmd);
            return NULL;
        }
        i++;
        t_lexer *next = lexer->next;
        rm_from_lexer(&lexer, lexer->index);
        lexer = next;
    }
    cmd[i] = NULL;
    return cmd;
}

t_simple_cmds *new_node_parser(t_lexer *lexer, t_pars_mini *pars_mini) {
    t_simple_cmds *new = malloc(sizeof(t_simple_cmds));
    if (!new) return NULL;

    new->redirections = pars_mini->redirections;
    new->num_redirections = pars_mini->num_redirections;
    new->str = build_command(lexer);
    new->next = NULL;
    new->prev = NULL;

    return new;
}

static t_simple_cmds *last_node(t_simple_cmds *lst) {
    if (lst) {
        while (lst->next != NULL)
            lst = lst->next;
    }
    return lst;
}

static void add_node_parser(t_simple_cmds **lst, t_simple_cmds *newnode) {
    t_simple_cmds *last;

    if (*lst) {
        last = last_node(*lst);
        last->next = newnode;
        newnode->prev = last;
    } else
        *lst = newnode;
}

void parser_part(t_shell *shell) {
    t_lexer *tmp = shell->lexer_list;
    t_pars_mini pars_mini;
    t_simple_cmds *commands = NULL;
    int count_pipe = shell->count_pipe + 1;

    while (count_pipe > 0) {
        pars_mini.redirections = NULL;
        pars_mini.num_redirections = 0;

        if (detach_redirections(&tmp, &pars_mini))
            break;
        if (!tmp)
            break;
        printf("pars mini redir - %s", pars_mini.redirections);
        printf("pars num redir - %d", pars_mini.num_redirections);

        add_node_parser(&commands, new_node_parser(tmp, &pars_mini));

        if (--count_pipe <= 0)
            break;

        while (tmp && tmp->token != PIPE)
            tmp = tmp->next;

        if (tmp && tmp->token == PIPE) {
            t_lexer *next_node = tmp->next;  // Сохраняем ссылку на следующий узел перед удалением
            rm_from_lexer(&tmp, tmp->index); // Удаляем текущий узел
            tmp = next_node;  // Обновляем tmp, чтобы указатель указывал на следующий элемент
}

    }
    shell->pars = commands;
}




/*
void parser_part(t_shell *shell) {
    t_lexer *tmp = shell->lexer_list;
    t_pars_mini *pars_mini;
    t_simple_cmds *commands;
    int i = 0;
    printf("1");
    int count_pipe = shell->count_pipe + 1;
    while (count_pipe > 0) {
        pars_mini->redirections = NULL;
        pars_mini->num_redirections = 0;
        printf("1");
        detach_redirections(&tmp, pars_mini);
        printf("2");
        if (pars_mini->lexer_list == NULL)
            return;

        tmp = shell->lexer_list;
        add_node_parser(&commands, new_node_parser(tmp, pars_mini));

        if (count_pipe == 0)
            break;

        while (tmp->token != PIPE)
            tmp = tmp->next;
        if (tmp->token == PIPE)
            rm_from_lexer(&tmp, tmp->index);
        
        count_pipe--;
    }
}*/
/*
int main(void) {
    // Initialize lexer with proper memory management
    t_lexer *lexer = malloc(sizeof(t_lexer));  // Create a single node and link them correctly
    if (!lexer) {
        return 1;  // Handle memory allocation failure
    }

    lexer[0] = (t_lexer){"cmd1", WORD, NULL, NULL};
    lexer[1] = (t_lexer){">", OUT, NULL, &lexer[0]};
    lexer[2] = (t_lexer){"file.txt", WORD, NULL, &lexer[1]};
    lexer[3] = (t_lexer){"|", PIPE, NULL, &lexer[2]};
    lexer[4] = (t_lexer){"cmd2", WORD, NULL, &lexer[3]};

    // Correctly link the next pointers
    lexer[0].next = &lexer[1];
    lexer[1].next = &lexer[2];
    lexer[2].next = &lexer[3];
    lexer[3].next = &lexer[4];

    t_pars_mini *pars_mini = malloc(sizeof(t_pars_mini));
    if (!pars_mini) {
        free_lexer_list(lexer);  // Ensure we free memory if allocation fails
        return 1;
    }
    pars_mini->redirections = NULL;
    pars_mini->num_redirections = 0;

    parser_part(1, lexer);  // Process the lexer list with redirection handling

    // Free all memory after processing
    free_pars_mini(pars_mini);
    free_lexer_list(lexer);

    return 0;
}
*/