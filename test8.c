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

void free_simple_cmds_list(t_simple_cmds **commands) {
    t_simple_cmds *current = *commands;
    t_simple_cmds *next;

    while (current != NULL) {
        next = current->next;

        if (current->str) {  // Check if the array of command strings exists
            for (int i = 0; current->str[i] != NULL; i++) {
                free(current->str[i]);  // Free each argument in the array
            }
            free(current->str);  // Free the array itself
        }

        if (current->redirections) {  // Free any associated redirections if they exist
            free_lexer_list(&current->redirections);
        }

        free(current);  // Free the command node itself
        current = next;
    }

    *commands = NULL;
}



char *ft_strdup(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);  // Allocate memory for the string + null terminator
    if (!copy) {
        return NULL;  // If malloc fails, return NULL
    }
    memcpy(copy, s, len + 1);  // Copy the string into the newly allocated memory
    return copy;
}

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

// Update free_lexer_list to prevent double-free
void free_lexer_list(t_lexer **lexer) {
    if (lexer == NULL || *lexer == NULL)
        return;

    t_lexer *current = *lexer;
    while (current) {
        t_lexer *next = current->next;
        
        if (current->str) {
            free(current->str);  // Free the string
            current->str = NULL;
        }
        
        free(current);  // Free the node
        current = next;
    }
    *lexer = NULL;
    printf("Lexer list freed.\n");
}

t_lexer *lexer_create(char *value, t_tokens token, int index) {
    t_lexer *new = (t_lexer *)malloc(sizeof(t_lexer));
    if (!new) return NULL;

    new->str = value;
    new->token = token;
    new->index = index;
    new->next = NULL;
    new->prev = NULL;
    printf("Created lexer node: token=%d, index=%d, value=%s\n", token, index, value);

    return new;
}

void rm_from_lexer(t_lexer **lexer, int index) {
    if (!lexer || !*lexer) return;

    t_lexer *node = *lexer;
    t_lexer *prev = NULL;

    while (node && node->index != index) {
        prev = node;
        node = node->next;
    }

    if (!node) return; // Node not found

    if (prev) {
        prev->next = node->next;
    } else {
        *lexer = node->next;
    }

    if (node->next) {
        node->next->prev = prev;
    }

    if (node->str) {
        free(node->str);
        node->str = NULL;
    }

    free(node);
    printf("Node with index %d removed from lexer.\n", index);
}


int add_new_redirection(t_lexer *to_add, t_lexer **lexer, t_pars_mini *utils) {
    if (!to_add || !to_add->next) return 1;

    t_lexer *newnode = lexer_create(ft_strdup(to_add->next->str), to_add->token, utils->num_redirections);  
    if (!newnode) return 1;

    lexer_add_back(&utils->redirections, newnode);

    int i1 = to_add->index;
    int i2 = to_add->next->index;

    rm_from_lexer(lexer, i1);
    rm_from_lexer(lexer, i2);

    utils->num_redirections++;
	//printf("num redir %d\n", utils->num_redirections);
	//printf("next str - %s, token - %d, num red - %d\n", to_add->next->str, to_add->token, utils->num_redirections);
    return 0;
}

int detach_redirections(t_lexer **lexer, t_pars_mini *utils) {
    t_lexer *current = *lexer;

    while (current && current->token != 1) {
        if (current->token > 1) {  // Assuming `> 1` identifies a redirection
            if (add_new_redirection(current, lexer, utils)) {
                return 1;
            }
            current = *lexer;  // Reset current to start after modifications
        } else {
            current = current->next;
        }
    }
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
		cmd[i] = ft_strdup(lexer->str); // Копируем содержимое узла в cmd[i]
		lexer = lexer->next;               // Переходим к следующему узлу
	}
	cmd[i] = NULL;                        // Завершаем массив, добавляя NULL в конец
	return (cmd);                         // Возвращаем массив строк, представляющий команду
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

void print_tokens(t_lexer *list) {
    t_lexer *tmp;

    tmp = list;

    while (tmp) {
        printf("Index: %d, Type: %d, Value: %s\n", tmp->index, tmp->token, tmp->str);
        tmp = tmp->next;
    }
}
void print_simple_cmds(t_simple_cmds *cmds) {
    printf("Start Simple Commands:\n");
    while (cmds) {
        printf("Command: ");
        for (int i = 0; cmds->str && cmds->str[i]; i++) {
            printf("%s ", cmds->str[i]);
        }
        printf("\n");
        
        printf("Number of redirections: %d\n", cmds->num_redirections);
        
        t_lexer *redir = cmds->redirections;
        while (redir) {
            printf("Redirection: Token: %d, Str: %s\n", redir->token, redir->str);
            redir = redir->next;
        }
        
        cmds = cmds->next;
    }
    printf("End of Simple Commands.\n");
}

void parser_part(int count_pipe, t_lexer *lexer_list) {
    t_lexer *tmp = lexer_list;
    t_pars_mini pars_mini;
    t_simple_cmds *commands = NULL;
    count_pipe = count_pipe + 1;

    while (count_pipe > 0 && tmp) {  // Add check for `tmp`
        pars_mini.redirections = NULL;
        pars_mini.num_redirections = 0;

        if (detach_redirections(&tmp, &pars_mini))
            break;

        // Verify if `tmp` is NULL after redirection detachment
        if (!tmp) break;

        t_simple_cmds *new_cmd = new_node_parser(tmp, &pars_mini);
        if (!new_cmd) {
            // Add proper error handling here
            break;
        }
        add_node_parser(&commands, new_cmd);
		//print_simple_cmds(commands);

        if (--count_pipe <= 0)
            break;

        // Move `tmp` to the next command segment after a `PIPE`
        while (tmp && tmp->token != PIPE)
            tmp = tmp->next;
        if (tmp && tmp->token == PIPE) 
            tmp = tmp->next;
    }

    // Free the lexer list if needed
	print_simple_cmds(commands);
    //free_lexer_list(&lexer_list);
	//free_simple_cmds_list(&commands);
}



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

