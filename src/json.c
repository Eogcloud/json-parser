// src/json.c
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Parsing context (tracks position in JSON string)
typedef struct {
    const char *json;
    size_t pos;
    size_t length;
} parse_context_t;

// Forward declarations for recursive parsing
static json_t* parse_value(parse_context_t *ctx);
static json_t* parse_object(parse_context_t *ctx);
static json_t* parse_array(parse_context_t *ctx);
static json_t* parse_string(parse_context_t *ctx);
static json_t* parse_number(parse_context_t *ctx);

// Memory management helpers
static json_t* json_new(void) {
    json_t *item = malloc(sizeof(json_t));
    if (item) {
        memset(item, 0, sizeof(json_t));  // Zero out all fields
    }
    return item;
}

// Skip whitespace in JSON
static void skip_whitespace(parse_context_t *ctx) {
    while (ctx->pos < ctx->length && 
           isspace(ctx->json[ctx->pos])) {
        ctx->pos++;
    }
}

// Peek at current character without advancing
static char peek_char(parse_context_t *ctx) {
    skip_whitespace(ctx);
    if (ctx->pos >= ctx->length) return '\0';
    return ctx->json[ctx->pos];
}

// Get current character and advance
static char next_char(parse_context_t *ctx) {
    skip_whitespace(ctx);
    if (ctx->pos >= ctx->length) return '\0';
    return ctx->json[ctx->pos++];
}

// Parse a JSON string value
static json_t* parse_string(parse_context_t *ctx) {
    if (next_char(ctx) != '"') return NULL;  // Must start with quote
    
    size_t start = ctx->pos;
    
    // Find closing quote (simplified - no escape handling yet)
    while (ctx->pos < ctx->length && ctx->json[ctx->pos] != '"') {
        ctx->pos++;
    }
    
    if (ctx->pos >= ctx->length) return NULL;  // Unclosed string
    
    // Create JSON string node
    json_t *item = json_new();
    if (!item) return NULL;
    
    item->type = JSON_STRING;
    
    // Copy string value
    size_t len = ctx->pos - start;
    item->valuestring = malloc(len + 1);
    if (!item->valuestring) {
        free(item);
        return NULL;
    }
    
    strncpy(item->valuestring, &ctx->json[start], len);
    item->valuestring[len] = '\0';
    
    ctx->pos++;  // Skip closing quote
    return item;
}

// Parse a JSON number
static json_t* parse_number(parse_context_t *ctx) {
    size_t start = ctx->pos;
    
    // Handle negative
    if (ctx->json[ctx->pos] == '-') {
        ctx->pos++;
    }
    
    // Parse integer part
    if (!isdigit(ctx->json[ctx->pos])) return NULL;
    
    while (ctx->pos < ctx->length && isdigit(ctx->json[ctx->pos])) {
        ctx->pos++;
    }
    
    // Handle decimal part (simplified)
    if (ctx->pos < ctx->length && ctx->json[ctx->pos] == '.') {
        ctx->pos++;
        while (ctx->pos < ctx->length && isdigit(ctx->json[ctx->pos])) {
            ctx->pos++;
        }
    }
    
    // Create JSON number node
    json_t *item = json_new();
    if (!item) return NULL;
    
    item->type = JSON_NUMBER;
    
    // Copy number string and convert to double
    size_t len = ctx->pos - start;
    char *number_str = malloc(len + 1);
    if (!number_str) {
        free(item);
        return NULL;
    }
    
    strncpy(number_str, &ctx->json[start], len);
    number_str[len] = '\0';
    
    item->valuenumber = atof(number_str);
    free(number_str);  // Clean up temporary string
    
    return item;
}

// Parse JSON literals (true, false, null)
static json_t* parse_literal(parse_context_t *ctx) {
    if (strncmp(&ctx->json[ctx->pos], "true", 4) == 0) {
        ctx->pos += 4;
        json_t *item = json_new();
        if (item) item->type = JSON_TRUE;
        return item;
    }
    
    if (strncmp(&ctx->json[ctx->pos], "false", 5) == 0) {
        ctx->pos += 5;
        json_t *item = json_new();
        if (item) item->type = JSON_FALSE;
        return item;
    }
    
    if (strncmp(&ctx->json[ctx->pos], "null", 4) == 0) {
        ctx->pos += 4;
        json_t *item = json_new();
        if (item) item->type = JSON_NULL;
        return item;
    }
    
    return NULL;  // No valid literal found
}

// Main value parser (dispatches to specific parsers)
static json_t* parse_value(parse_context_t *ctx) {
    char c = peek_char(ctx);
    
    switch (c) {
        case '"':  return parse_string(ctx);
        case '{':  return parse_object(ctx);
        case '[':  return parse_array(ctx);
        case 't':
        case 'f':
        case 'n':  return parse_literal(ctx);
        default:
            if (c == '-' || isdigit(c)) {
                return parse_number(ctx);
            }
            return NULL;  // Invalid character
    }
}

// Parse a JSON object
static json_t* parse_object(parse_context_t *ctx) {
    if (next_char(ctx) != '{') return NULL;  // Must start with '{'
    
    json_t *object = json_new();
    if (!object) return NULL;
    object->type = JSON_OBJECT;
    
    // Handle empty object
    if (peek_char(ctx) == '}') {
        next_char(ctx);  // Consume '}'
        return object;
    }
    
    json_t *current_child = NULL;
    
    while (1) {
        // Parse key (must be a string)
        json_t *key_item = parse_string(ctx);
        if (!key_item) {
            json_delete(object);
            return NULL;
        }
        
        // Key becomes the property name, not a separate value
        char *key = key_item->valuestring;
        key_item->valuestring = NULL;  // Transfer ownership
        json_delete(key_item);
        
        // Expect colon
        if (next_char(ctx) != ':') {
            free(key);
            json_delete(object);
            return NULL;
        }
        
        // Parse value
        json_t *value_item = parse_value(ctx);
        if (!value_item) {
            free(key);
            json_delete(object);
            return NULL;
        }
        
        // Set the key name on the value item
        value_item->string = key;
        
        // Add to object's child list
        if (!object->child) {
            // First child
            object->child = value_item;
            current_child = value_item;
        } else {
            // Add to end of list
            current_child->next = value_item;
            value_item->prev = current_child;
            current_child = value_item;
        }
        
        // Check for continuation
        char next = peek_char(ctx);
        if (next == '}') {
            next_char(ctx);  // Consume '}'
            break;
        } else if (next == ',') {
            next_char(ctx);  // Consume ','
            continue;
        } else {
            // Invalid character
            json_delete(object);
            return NULL;
        }
    }
    
    return object;
}

static json_t* parse_array(parse_context_t *ctx) {
    // TODO: Implement array parsing  
    (void)ctx;  // Suppress unused parameter warning
    return NULL;
}

// Main parsing function
json_t* json_parse(const char *text) {
    if (!text) return NULL;
    
    parse_context_t ctx = {
        .json = text,
        .pos = 0,
        .length = strlen(text)
    };
    
    return parse_value(&ctx);
}

// Memory cleanup
void json_delete(json_t *json) {
    if (!json) return;
    
    // Recursively delete children
    json_t *child = json->child;
    while (child) {
        json_t *next = child->next;
        json_delete(child);
        child = next;
    }
    
    // Free string data
    if (json->valuestring) free(json->valuestring);
    if (json->string) free(json->string);
    
    // Free the node itself
    free(json);
}

// Helper functions for accessing objects and arrays
json_t* json_object_get(const json_t *object, const char *key) {
    if (!object || !key || object->type != JSON_OBJECT) return NULL;
    
    json_t *child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, key) == 0) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

json_t* json_array_get(const json_t *array, int index) {
    if (!array || index < 0 || array->type != JSON_ARRAY) return NULL;
    
    json_t *child = array->child;
    for (int i = 0; i < index && child; i++) {
        child = child->next;
    }
    return child;
}

int json_array_size(const json_t *array) {
    if (!array || array->type != JSON_ARRAY) return 0;
    
    int count = 0;
    json_t *child = array->child;
    while (child) {
        count++;
        child = child->next;
    }
    return count;
}

// Type checking helpers
int json_is_false(const json_t *json) {
    return json && json->type == JSON_FALSE;
}

int json_is_true(const json_t *json) {
    return json && json->type == JSON_TRUE;
}

int json_is_bool(const json_t *json) {
    return json && (json->type == JSON_TRUE || json->type == JSON_FALSE);
}

int json_is_null(const json_t *json) {
    return json && json->type == JSON_NULL;
}

int json_is_number(const json_t *json) {
    return json && json->type == JSON_NUMBER;
}

int json_is_string(const json_t *json) {
    return json && json->type == JSON_STRING;
}

int json_is_array(const json_t *json) {
    return json && json->type == JSON_ARRAY;
}

int json_is_object(const json_t *json) {
    return json && json->type == JSON_OBJECT;
}