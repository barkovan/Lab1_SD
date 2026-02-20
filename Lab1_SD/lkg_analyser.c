#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 32

void cmd_get_c()
{
    // pass
}

void cmd_get_a()
{
    // pass
}

void cmd_lcg()
{
    // pass
}

void cmd_test()
{
    // pass
}

// Read a string of indeterminate length
char* read_line(FILE* file) {
    char* str = NULL;
    size_t len = 0;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, file)) {
        size_t chunk_len = strlen(buffer);

        if (chunk_len && buffer[chunk_len - 1] == '\n') {
            buffer[--chunk_len] = '\0';
        }

        char* tmp = realloc(str, len + chunk_len + 1);
        if (!tmp) {
            free(str);
            return NULL;
        }
        str = tmp;

        memcpy(str + len, buffer, chunk_len);
        len += chunk_len;
        str[len] = '\0';

        if (chunk_len < BUFFER_SIZE - 1)
            break;
    }

    if (len == 0) {
        free(str);
        return NULL;
    }

    return str;
}

// Select a command from the line
char* extract_command(const char* line)
{
    if (!line || !*line) return NULL;

    while (*line && isspace(*line)) line++;
    if (!*line) return NULL;

    const char* start = line;
    while (*line && !isspace(*line)) line++;

    size_t len = line - start;
    char* cmd = malloc(len + 1);
    if (!cmd) return NULL;

    strncpy(cmd, start, len);
    cmd[len] = '\0';
    return cmd;
}

// Check if the command is known
int is_valid_command(const char* cmd)
{
    const char* valid_commands[] = { "get_c", "get_a", "lcg", "test", NULL };

    for (int i = 0; valid_commands[i]; i++)
    {
        if (strcmp(cmd, valid_commands[i]) == 0)
            return 1;
    }
    return 0;
}

// Get arguments
const char* get_arguments(const char* line)
{
    while (*line && !isspace(*line)) line++;
    while (*line && isspace(*line)) line++;
    return line;
}

int main()
{
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    if (!input || !output) {
        if (input) fclose(input);
        if (output) fclose(output);
        return 1;
    }

    char* line = read_line(input);
    if (!line) {
        fprintf(output, "incorrect command\n");
        fclose(input);
        fclose(output);
        return 0;
    }

    char* cmd = extract_command(line);
    if (!cmd || !is_valid_command(cmd))
    {
        fprintf(output, "incorrect command\n");
        free(cmd);
        free(line);
        fclose(input);
        fclose(output);
        return 0;
    }

    const char* args = get_arguments(line);

    if (strcmp(cmd, "get_c") == 0)
    {
        cmd_get_c(args);
    }
    else if (strcmp(cmd, "get_a") == 0)
    {
        cmd_get_a(args);
    }
    else if (strcmp(cmd, "lcg") == 0)
    {
        cmd_lcg(args);
    }
    else if (strcmp(cmd, "test") == 0)
    {
        cmd_test(args);
    }

    free(cmd);
    free(line);
    fclose(input);
    fclose(output);
    return 0;
}