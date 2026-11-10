#include "text_process_pipeline/lexer.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// ---------- helpers ----------

// Copy text[start..end) into a new null-terminated string.
static char *copy_range(const char *text, size_t start, size_t end)
{
    size_t subtext_len = end - start;
    char  *out = malloc((subtext_len + 1) * sizeof(*out));

    if (out == NULL)
    {
        perror("meBash: malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(out, text + start, subtext_len);
    out[subtext_len] = '\0';            // index into OUT, not the input
    return out;
}

// Append a token; the list takes ownership of `value`.
static void push(TokenList *list, TokenType type, char *value, size_t pos)
{
    if (list->count == list->capacity)
    {
        size_t new_cap = list->capacity ? list->capacity * 2
                                        : TOKEN_LIST_CAPACITY;  // initial size
        Token *bigger = realloc(list->tokens, new_cap * sizeof(*bigger));

        if (bigger == NULL)
        {
            perror("meBash: realloc");
            exit(EXIT_FAILURE);
        }
        list->tokens = bigger;
        list->capacity = new_cap;
    }
    list->tokens[list->count].type = type;
    list->tokens[list->count].value = value;
    list->tokens[list->count].start = pos;
    list->count++;
}

void free_tokens(TokenList *list)
{
    for (size_t i = 0; i < list->count; i++)
        free(list->tokens[i].value);
    free(list->tokens);
    list->tokens = NULL;
    list->count = 0;
    list->capacity = 0;
}

static int is_op_char(char c)
{
    return c == '|' || c == '&' || c == ';' || c == '<' || c == '>';
}

static int is_space(char c)
{
    return isspace((unsigned char)c);   // cast avoids UB on negative chars
}

// Longest match first. Safe at end of string: s[1] is then '\0'.
static size_t match_operator(const char *s, TokenType *type)
{
    if (s[0] == '|' && s[1] == '|') { *type = TOK_OR;         return 2; }
    if (s[0] == '&' && s[1] == '&') { *type = TOK_AND;        return 2; }
    if (s[0] == '<' && s[1] == '<') { *type = TOK_HEREDOC;    return 2; }
    if (s[0] == '>' && s[1] == '>') { *type = TOK_APPEND;     return 2; }
    if (s[0] == '|')                { *type = TOK_PIPE;       return 1; }
    if (s[0] == '&')                { *type = TOK_BG;         return 1; }
    if (s[0] == ';')                { *type = TOK_SEMI;       return 1; }
    if (s[0] == '<')                { *type = TOK_REDIR_IN;   return 1; }
    if (s[0] == '>')                { *type = TOK_REDIR_OUT;  return 1; }
    return 0;
}

// text[*i] is the opening quote. On success, moves *i just past the closing
// quote and returns 0. If unterminated, leaves *i alone and returns -1.
static int skip_quoted(const char *text, size_t len, size_t *i)
{
    char   quote = text[*i];
    size_t j = *i + 1;

    while (j < len && text[j] != quote)
    {
        if (quote == '"' && text[j] == '\\' && j + 1 < len)
            j += 2;                     // \" inside double quotes
        else
            j += 1;                     // inside '...' backslash is plain text
    }
    if (j >= len)
        return -1;
    *i = j + 1;
    return 0;
}

void print_tokens(const TokenList *list)
{
    static const char *names[] = {
        "WORD", "PIPE", "OR", "BG", "AND", "SEMI",
        "REDIR_IN", "HEREDOC", "REDIR_OUT", "APPEND"
    };

    for (size_t i = 0; i < list->count; i++)
        printf("%-9s %-16s (pos %zu)\n", names[list->tokens[i].type],
               list->tokens[i].value, list->tokens[i].start);
}

// ---------- lexer ----------

// Returns 0 on success, -1 on error (message in err, list already freed).
int lex(const char *text, TokenList *out, char *err, size_t err_size)
{
    size_t text_len = strlen(text);
    size_t position = 0;

    out->tokens = NULL;
    out->count = 0;
    out->capacity = 0;

    while (position < text_len)
    {
        // Skip whitespace
        if (is_space(text[position]))
        {
            position++;
            continue;
        }

        // Operators
        if (is_op_char(text[position]))
        {
            TokenType type;
            size_t    n = match_operator(text + position, &type);

            push(out, type, copy_range(text, position, position + n), position);
            position += n;
            continue;
        }

        // Words
        size_t start = position;

        while (position < text_len && !is_space(text[position])
                                   && !is_op_char(text[position]))
        {
            // Backslash outside quotes: skip it and the next char together
            if (text[position] == '\\')
            {
                position += (position + 1 < text_len) ? 2 : 1;
                continue;
            }

            // Quoted section in the middle of a word
            if (text[position] == '\'' || text[position] == '"')
            {
                size_t open_quote_position = position;
                char   q = text[position];

                if (skip_quoted(text, text_len, &position) != 0)
                {
                    snprintf(err, err_size,
                             "Unterminated %c quote starting at position %zu",
                             q, open_quote_position);
                    free_tokens(out);
                    return -1;
                }
                continue;
            }

            position++;
        }
        push(out, TOK_WORD, copy_range(text, start, position), start);
    }
    return 0;
}