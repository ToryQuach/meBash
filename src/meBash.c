#include "text_process_pipeline/input.h"
#include "text_process_pipeline/lexer.h"
// #include "text_process_pipeline/expand.h"
// #include "text_process_pipeline/parser.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char      *input;
    TokenList  tokens;
    char       err[128];

    while (1)
    {
        printf("> ");
        fflush(stdout);
        input = get_user_input_text();

        if (input == NULL)
        {
            printf("\n");
            break;
        }
        if (input[0] == '\0')
        {
            free(input);
            continue;
        }
        if (lex(input, &tokens, err, sizeof err) != EXIT_SUCCESS)
            fprintf(stderr, "%s\n", err);
        else
        {
            print_tokens(&tokens);
            free_tokens(&tokens);
        }
        free(input);
    }
    return EXIT_SUCCESS;
}