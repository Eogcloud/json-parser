#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *program_name) {
    printf("Usage: %s [options] [file]\n", program_name);
    printf("JSON parser and validator\n\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --validate Validate JSON only (exit code 0=valid, 1=invalid)\n");
    printf("  -p, --pretty   Pretty print JSON (TODO)\n");
    printf("  -              Read from stdin\n\n");
    printf("Examples:\n");
    printf("  %s file.json                 # Parse and validate file.json\n", program_name);
    printf("  echo '{\"test\": 42}' | %s -   # Parse from stdin\n", program_name);
    printf("  %s -v file.json              # Just validate (silent)\n", program_name);
}

char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, size, file);
    content[bytes_read] = '\0';
    
    fclose(file);
    return content;
}

char* read_stdin() {
    char *content = NULL;
    size_t size = 0;
    size_t capacity = 1024;
    
    content = malloc(capacity);
    if (!content) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    int c;
    while ((c = getchar()) != EOF) {
        if (size >= capacity - 1) {
            capacity *= 2;
            char *new_content = realloc(content, capacity);
            if (!new_content) {
                free(content);
                fprintf(stderr, "Error: Memory allocation failed\n");
                return NULL;
            }
            content = new_content;
        }
        content[size++] = c;
    }
    
    content[size] = '\0';
    return content;
}

void print_json_info(const json_t *json, int indent) {
    if (!json) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (json->type) {
        case JSON_STRING:
            printf("STRING: \"%s\"\n", json->valuestring);
            break;
        case JSON_NUMBER:
            printf("NUMBER: %.2f\n", json->valuenumber);
            break;
        case JSON_TRUE:
            printf("BOOLEAN: true\n");
            break;
        case JSON_FALSE:
            printf("BOOLEAN: false\n");
            break;
        case JSON_NULL:
            printf("NULL\n");
            break;
        case JSON_OBJECT:
            printf("OBJECT (TODO: not implemented yet)\n");
            break;
        case JSON_ARRAY:
            printf("ARRAY (TODO: not implemented yet)\n");
            break;
        default:
            printf("UNKNOWN TYPE\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    int validate_only = 0;
    int from_stdin = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--validate") == 0) {
            validate_only = 1;
        } else if (strcmp(argv[i], "-") == 0) {
            from_stdin = 1;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    char *json_text = NULL;
    if (from_stdin) {
        json_text = read_stdin();
    } else if (input_file) {
        json_text = read_file(input_file);
    } else {
        fprintf(stderr, "Error: No input specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    if (!json_text) {
        return 1;
    }
    
    json_t *parsed = json_parse(json_text);
    
    if (!parsed) {
        if (!validate_only) {
            fprintf(stderr, "Error: Invalid JSON\n");
        }
        free(json_text);
        return 1;
    }
    
    if (validate_only) {
    } else {
        printf("JSON parsed successfully!\n");
        printf("Root type: ");
        print_json_info(parsed, 0);
    }
    
    json_delete(parsed);
    free(json_text);
    return 0;
}