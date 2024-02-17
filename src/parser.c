
#include "headers/parser.h"

#include "headers/environment.h"
#include "headers/lexer.h"
#include "lexer.c"

void free_tokens(Token* root) {
    while (root) {
        Token* current = root;
        root = root->next;
        free(current);
    }
}

void print_tokens_ll(Token* root) {
    size_t count = 1;
    while (root) {
        if (count > 1000) {
            break;
        }
        printf("Token %zu : ", count);
        if (root->beginning && root->end) {
            printf("%.*s\n", (int)(root->end - root->beginning),
                   root->beginning);
        }
        root = root->next;
        count++;
    }
}

Error parse(char* contents, Node result) {
    Token* tokens = NULL;
    Token* token_it = tokens;
    Token current;
    current.next = NULL;
    current.beginning = contents;
    current.end = contents;
    Error err = OK;
    while ((err = lex(current.end, &current)).type == ERROR_NULL) {
        if ((current.end - current.beginning == 0)) {
            break;
        }
        if (tokens) {
            token_it->next = create_token();
            memcpy(token_it->next, &current, sizeof(Token));
            token_it = token_it->next;
        } else {
            tokens = create_token();
            memcpy(tokens, &current, sizeof(Token));
            token_it = tokens;
        }
    }

    print_tokens_ll(tokens);

    // LOGIC

    free_tokens(tokens);

    return err;
}