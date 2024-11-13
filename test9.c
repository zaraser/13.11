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

/*void free_simple_cmds_list(t_simple_cmds **commands)
{
    t_simple_cmds *current;
    t_simple_cmds *next;
    int i;

    i = 0;
    current = *commands;
    while (current != NULL)
    {
        next = current->next;
        if (current->str)
        {
            while (current->str[i] != NULL)
            {
                free(current->str[i]);
                current->str[i] = NULL;
                i++;
            }
            free(current->str);
            current->str = NULL;
        }
        if (current->redirections)
            free_lexer_list(&current->redirections);
        free(current);
        current = NULL;
        current = next;
    }
    *commands = NULL;
    printf("Commands list freed.\n");
}*/

void free_redirections(t_lexer *redirection) {
    t_lexer *current = redirection;
    while (current) {
        t_lexer *next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

void free_simple_cmds_list(t_simple_cmds *commands)
{
    t_simple_cmds *current;
    int i;

    while (commands)
    {
        current = commands;
        commands = commands->next;

        if (current->str)
        {
            // Освобождаем каждую строку в массиве cmd
            for (i = 0; current->str[i]; i++)
            {
                free(current->str[i]);
                current->str[i] = NULL;  // Обнуление после освобождения
            }
            free(current->str); // Освобождаем сам массив cmd
            current->str = NULL;  // Обнуление для предотвращения повторного доступа
        }

        // Освобождаем память, выделенную для redirections, если они есть
        free_redirections(current->redirections);
        current->redirections = NULL;

        free(current); // Освобождаем сам узел команды
    }
}


char *ft_strdup(const char *s)
{
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy)
        return (NULL);
    memcpy(copy, s, len + 1);
    return copy;
}

void rm_from_lexer(t_lexer **lexer, int index)
{
    t_lexer *node;
    t_lexer *prev;

    if (!lexer || !*lexer)
        return ;
    node = *lexer;
    prev = NULL;
    while (node && node->index != index)
    {
        prev = node;
        node = node->next;
    }
    if (!node)
        return ;
    if (prev)
        prev->next = node->next;
    else
        *lexer = node->next;
    if (node->next)
        node->next->prev = prev;
    if (node->str)
    {
        free(node->str);
        node->str = NULL;
    }
    free(node);
    node = NULL;
    printf("Node with index %d removed from lexer.\n", index);
}

int add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_pars_mini *utils)
{
    t_lexer *newnode;

    if (!to_add || !to_add->next)
        return (1);
    newnode = lexer_create(ft_strdup(to_add->next->str), to_add->token, utils->num_redirections);  
    if (!newnode)
        return (1);
    lexer_add_back(&utils->redirections, newnode);
    int i1 = to_add->index;
    int i2 = to_add->next->index;
    rm_from_lexer(lexer, i1);
    rm_from_lexer(lexer, i2);
    utils->num_redirections++;
    return (0);
}

int detach_redirections(t_lexer **lexer, t_pars_mini *utils)
{
    t_lexer *current;

    current = *lexer;
    while (current && current->token != 1)
    {
        if (current->token > 1)
        {
            if (add_new_redirection(current, lexer, utils))
                return (1);
            current = *lexer;
        }
        else
            current = current->next;
    }
    return (0);
}

int count_nodes(t_lexer *lexer)
{
    int res;

    res = 0;
    while (lexer && lexer->token != PIPE)
    {
        ++res;
        lexer = lexer->next;
    }
    return (res);
}

void free_command(char **cmd)
{
    int i = 0;

    if (!cmd)
        return;
    while (cmd[i])
    {
        free(cmd[i]);
        i++;
    }
    free(cmd);
}


char **build_command(t_lexer *lexer)
{
    char **cmd;
    int i;
    int len;

    if (!lexer)
        return (NULL);
    len = count_nodes(lexer);
    cmd = malloc(sizeof(char *) * (len + 1));
    if (!cmd)
        return (NULL);
    i = 0;
    while (i < len)
    {
        cmd[i] = ft_strdup(lexer->str);
        if (!cmd[i]) // Проверка на успешное выделение памяти
        {
            free_command(cmd); // Освобождаем все уже выделенные строки
            return (NULL);
        }
        lexer = lexer->next;
        i++;
    }
    cmd[i] = NULL;
    return (cmd);
}

t_simple_cmds *new_node_parser(t_lexer *lexer, t_pars_mini *pars_mini)
{
    t_simple_cmds *new;

    new = malloc(sizeof(t_simple_cmds));
    if (!new)
        return NULL;
    new->redirections = pars_mini->redirections;
    new->num_redirections = pars_mini->num_redirections;
    new->str = build_command(lexer);
    new->next = NULL;
    new->prev = NULL;
    return (new);
}

static t_simple_cmds *last_node(t_simple_cmds *lst)
{
    if (lst)
    {
        while (lst->next != NULL)
            lst = lst->next;
    }
    return (lst);
}

static void add_node_parser(t_simple_cmds **lst, t_simple_cmds *newnode)
{
    t_simple_cmds *last;

    if (*lst)
    {
        last = last_node(*lst);
        last->next = newnode;
        newnode->prev = last;
    }
    else
        *lst = newnode;
}

void print_simple_cmds(t_simple_cmds *cmds)
{
    printf("Start Simple Commands:\n");
    while (cmds) {
        printf("Command: ");
        for (int i = 0; cmds->str && cmds->str[i]; i++)
        {
            printf("%s ", cmds->str[i]);
        }
        printf("\n");
        printf("Number of redirections: %d\n", cmds->num_redirections);
        t_lexer *redir = cmds->redirections;
        while (redir)
        {
            printf("Redirection: Token: %d, Str: %s\n", redir->token, redir->str);
            redir = redir->next;
        }
        cmds = cmds->next;
    }
    printf("End of Simple Commands.\n");
}

void parser_part(int count_pipe, t_lexer *lexer_list)
{
    t_lexer *tmp;
    t_pars_mini pars_mini;
    int i;

    i = 0;
    tmp = lexer_list;
    t_simple_cmds *commands = NULL;
    count_pipe = count_pipe + 1;
    while (count_pipe > 0 && tmp)
    {
        pars_mini.redirections = NULL;
        pars_mini.num_redirections = 0;
        if (detach_redirections(&tmp, &pars_mini))
        {
            printf("Error detaching redirections\n");
            break;
        }
        if (!i++)
            lexer_list = tmp;
        t_simple_cmds *new_cmd = new_node_parser(tmp, &pars_mini);
        if (!new_cmd)
        {
            printf("Error creating new command node\n");
            break;
        }
        add_node_parser(&commands, new_cmd);
        if (!tmp)
            break;
        if (--count_pipe <= 0)
            break;
        while (tmp && tmp->token != PIPE)
            tmp = tmp->next;
        if (tmp && tmp->token == PIPE) 
            tmp = tmp->next;
    }
    print_simple_cmds(commands);
    free_lexer_list(&lexer_list);
    lexer_list = NULL;
    free_simple_cmds_list(commands);
}



/*
int main(void) {
    // Initialize a linked list for lexer nodes with dynamic allocation
    t_lexer *lexer_list = NULL;

    // Create and link each node individually
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("cmd1"), WORD, 0));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("<"), IN, 1));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("file.txt"), WORD, 2));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("|"), PIPE, 3));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("cmd3"), WORD, 4));
	lexer_add_back(&lexer_list, lexer_create(ft_strdup("|"), PIPE, 5));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("cmd2"), WORD, 6));
	lexer_add_back(&lexer_list, lexer_create(ft_strdup(">"), OUT, 7));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("file1.txt"), WORD, 8));
	 lexer_add_back(&lexer_list, lexer_create(ft_strdup(">"), OUT, 9));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("file2.txt"), WORD, 10));
		lexer_add_back(&lexer_list, lexer_create(ft_strdup("|"), PIPE, 11));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("cmd4"), WORD, 12));
	lexer_add_back(&lexer_list, lexer_create(ft_strdup("<"), IN, 13));
    lexer_add_back(&lexer_list, lexer_create(ft_strdup("file3.txt"), WORD, 14));
	    lexer_add_back(&lexer_list, lexer_create(ft_strdup("ca"), WORD, 15));
	print_tokens(lexer_list);
    parser_part(11, lexer_list);  // Process the lexer list with redirection handling
	

    return 0;
}

*/