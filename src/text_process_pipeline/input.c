#include "text_process_pipeline/input.h"
#include <stdlib.h>
#include <stdio.h>

char* get_user_input_text(void){
    int buffer_size = INPUT_BUFFER_SIZE;
    int position = 0;
    char* buffer = malloc(buffer_size * sizeof(char));
    char* bigger;
    int c;

    if (buffer == NULL){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        // Terminate if hit EOF at 0th position
        if (c == EOF && position == 0){
            free(buffer);
            return NULL;
        }

        // Terminate if hit EOF or \n
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        buffer[position] = (char)c;
        ++position;

        if (position >= buffer_size){
            buffer_size += INPUT_BUFFER_SIZE;
            bigger = realloc(buffer, buffer_size);
            if (bigger == NULL){
                free(buffer);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
            buffer = bigger;
        }
    }
    
}