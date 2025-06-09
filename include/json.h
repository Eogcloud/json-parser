#ifndef JSON_H
#define JSON_H

#include <stddef.h>

#define JSON_INVALID 0
#define JSON_FALSE   1
#define JSON_TRUE    2
#define JSON_NULL    4
#define JSON_NUMBER  8
#define JSON_STRING  16
#define JSON_ARRAY   32
#define JSON_OBJECT  64

typedef struct json {
    struct json *next;      
    struct json *prev;      
    struct json *child;     
    int type;             
    char *valuestring;     
    double valuenumber;     
    char *string;          
} json_t;

json_t* json_parse(const char *text);
void json_delete(json_t *json);
char* json_print(const json_t *json);

json_t* json_object_get(const json_t *object, const char *key);
json_t* json_array_get(const json_t *array, int index);
int json_array_size(const json_t *array);

int json_is_false(const json_t *json);
int json_is_true(const json_t *json);
int json_is_bool(const json_t *json);
int json_is_null(const json_t *json);
int json_is_number(const json_t *json);
int json_is_string(const json_t *json);
int json_is_array(const json_t *json);
int json_is_object(const json_t *json);

#endif