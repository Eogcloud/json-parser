// tests/test_objects.c
#include "unity/unity.h"
#include "../include/json.h"

void setUp(void) {}
void tearDown(void) {}

// Test empty object
void test_parse_empty_object(void) {
    json_t *result = json_parse("{}");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_OBJECT, result->type);
    TEST_ASSERT_NULL(result->child);  // No children
    
    json_delete(result);
}

// Test simple object with one key-value pair
void test_parse_simple_object(void) {
    json_t *result = json_parse("{\"name\": \"John\"}");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_OBJECT, result->type);
    TEST_ASSERT_NOT_NULL(result->child);
    
    // Check the name property
    json_t *name = json_object_get(result, "name");
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL(JSON_STRING, name->type);
    TEST_ASSERT_EQUAL_STRING("John", name->valuestring);
    TEST_ASSERT_EQUAL_STRING("name", name->string);  // Key name
    
    json_delete(result);
}

// Test object with multiple properties
void test_parse_multi_property_object(void) {
    json_t *result = json_parse("{\"name\": \"John\", \"age\": 30, \"active\": true}");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_OBJECT, result->type);
    
    // Check name
    json_t *name = json_object_get(result, "name");
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL(JSON_STRING, name->type);
    TEST_ASSERT_EQUAL_STRING("John", name->valuestring);
    
    // Check age
    json_t *age = json_object_get(result, "age");
    TEST_ASSERT_NOT_NULL(age);
    TEST_ASSERT_EQUAL(JSON_NUMBER, age->type);
    TEST_ASSERT_EQUAL_DOUBLE(30.0, age->valuenumber);
    
    // Check active
    json_t *active = json_object_get(result, "active");
    TEST_ASSERT_NOT_NULL(active);
    TEST_ASSERT_EQUAL(JSON_TRUE, active->type);
    
    // Check non-existent key
    json_t *missing = json_object_get(result, "missing");
    TEST_ASSERT_NULL(missing);
    
    json_delete(result);
}

// Test object with whitespace
void test_parse_object_with_whitespace(void) {
    json_t *result = json_parse("{ \"key\" : \"value\" }");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_OBJECT, result->type);
    
    json_t *value = json_object_get(result, "key");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("value", value->valuestring);
    
    json_delete(result);
}

// Test type checking functions
void test_type_checking(void) {
    json_t *obj = json_parse("{}");
    json_t *str = json_parse("\"test\"");
    json_t *num = json_parse("42");
    json_t *bool_true = json_parse("true");
    json_t *bool_false = json_parse("false");
    json_t *null_val = json_parse("null");
    
    TEST_ASSERT_TRUE(json_is_object(obj));
    TEST_ASSERT_FALSE(json_is_string(obj));
    
    TEST_ASSERT_TRUE(json_is_string(str));
    TEST_ASSERT_FALSE(json_is_number(str));
    
    TEST_ASSERT_TRUE(json_is_number(num));
    TEST_ASSERT_FALSE(json_is_bool(num));
    
    TEST_ASSERT_TRUE(json_is_true(bool_true));
    TEST_ASSERT_TRUE(json_is_bool(bool_true));
    TEST_ASSERT_FALSE(json_is_false(bool_true));
    
    TEST_ASSERT_TRUE(json_is_false(bool_false));
    TEST_ASSERT_TRUE(json_is_bool(bool_false));
    TEST_ASSERT_FALSE(json_is_true(bool_false));
    
    TEST_ASSERT_TRUE(json_is_null(null_val));
    TEST_ASSERT_FALSE(json_is_string(null_val));
    
    json_delete(obj);
    json_delete(str);
    json_delete(num);
    json_delete(bool_true);
    json_delete(bool_false);
    json_delete(null_val);
}

// Test error cases
void test_object_error_cases(void) {
    // Missing closing brace
    TEST_ASSERT_NULL(json_parse("{\"key\": \"value\""));
    
    // Missing colon
    TEST_ASSERT_NULL(json_parse("{\"key\" \"value\"}"));
    
    // Missing value
    TEST_ASSERT_NULL(json_parse("{\"key\":}"));
    
    // Trailing comma
    TEST_ASSERT_NULL(json_parse("{\"key\": \"value\",}"));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_parse_empty_object);
    RUN_TEST(test_parse_simple_object);
    RUN_TEST(test_parse_multi_property_object);
    RUN_TEST(test_parse_object_with_whitespace);
    RUN_TEST(test_type_checking);
    RUN_TEST(test_object_error_cases);
    
    return UNITY_END();
}