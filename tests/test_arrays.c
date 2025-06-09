// tests/test_arrays.c
#include "unity/unity.h"
#include "../include/json.h"

void setUp(void) {}
void tearDown(void) {}

// Test empty array
void test_parse_empty_array(void) {
    json_t *result = json_parse("[]");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_ARRAY, result->type);
    TEST_ASSERT_NULL(result->child);  // No children
    TEST_ASSERT_EQUAL_INT(0, json_array_size(result));
    
    json_delete(result);
}

// Test single element array
void test_parse_single_element_array(void) {
    json_t *result = json_parse("[42]");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_ARRAY, result->type);
    TEST_ASSERT_NOT_NULL(result->child);
    TEST_ASSERT_EQUAL_INT(1, json_array_size(result));
    
    // Check first element
    json_t *first = json_array_get(result, 0);
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_EQUAL(JSON_NUMBER, first->type);
    TEST_ASSERT_EQUAL_DOUBLE(42.0, first->valuenumber);
    
    // Check out of bounds
    json_t *invalid = json_array_get(result, 1);
    TEST_ASSERT_NULL(invalid);
    
    json_delete(result);
}

// Test array with multiple elements of same type
void test_parse_number_array(void) {
    json_t *result = json_parse("[1, 2, 3, 4, 5]");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_ARRAY, result->type);
    TEST_ASSERT_EQUAL_INT(5, json_array_size(result));
    
    // Check all elements
    for (int i = 0; i < 5; i++) {
        json_t *elem = json_array_get(result, i);
        TEST_ASSERT_NOT_NULL(elem);
        TEST_ASSERT_EQUAL(JSON_NUMBER, elem->type);
        TEST_ASSERT_EQUAL_DOUBLE((double)(i + 1), elem->valuenumber);
    }
    
    json_delete(result);
}

// Test array with mixed types
void test_parse_mixed_array(void) {
    json_t *result = json_parse("[\"hello\", 42, true, null, false]");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_ARRAY, result->type);
    TEST_ASSERT_EQUAL_INT(5, json_array_size(result));
    
    // Check string
    json_t *str = json_array_get(result, 0);
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL(JSON_STRING, str->type);
    TEST_ASSERT_EQUAL_STRING("hello", str->valuestring);
    
    // Check number
    json_t *num = json_array_get(result, 1);
    TEST_ASSERT_NOT_NULL(num);
    TEST_ASSERT_EQUAL(JSON_NUMBER, num->type);
    TEST_ASSERT_EQUAL_DOUBLE(42.0, num->valuenumber);
    
    // Check true
    json_t *bool_true = json_array_get(result, 2);
    TEST_ASSERT_NOT_NULL(bool_true);
    TEST_ASSERT_EQUAL(JSON_TRUE, bool_true->type);
    
    // Check null
    json_t *null_val = json_array_get(result, 3);
    TEST_ASSERT_NOT_NULL(null_val);
    TEST_ASSERT_EQUAL(JSON_NULL, null_val->type);
    
    // Check false
    json_t *bool_false = json_array_get(result, 4);
    TEST_ASSERT_NOT_NULL(bool_false);
    TEST_ASSERT_EQUAL(JSON_FALSE, bool_false->type);
    
    json_delete(result);
}

// Test array with whitespace
void test_parse_array_with_whitespace(void) {
    json_t *result = json_parse("[ 1 , 2 , 3 ]");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_ARRAY, result->type);
    TEST_ASSERT_EQUAL_INT(3, json_array_size(result));
    
    for (int i = 0; i < 3; i++) {
        json_t *elem = json_array_get(result, i);
        TEST_ASSERT_NOT_NULL(elem);
        TEST_ASSERT_EQUAL_DOUBLE((double)(i + 1), elem->valuenumber);
    }
    
    json_delete(result);
}

// Test nested structures (arrays in objects, objects in arrays)
void test_parse_nested_structures(void) {
    // Object containing an array
    json_t *obj_with_array = json_parse("{\"numbers\": [1, 2, 3]}");
    
    TEST_ASSERT_NOT_NULL(obj_with_array);
    TEST_ASSERT_EQUAL(JSON_OBJECT, obj_with_array->type);
    
    json_t *numbers = json_object_get(obj_with_array, "numbers");
    TEST_ASSERT_NOT_NULL(numbers);
    TEST_ASSERT_EQUAL(JSON_ARRAY, numbers->type);
    TEST_ASSERT_EQUAL_INT(3, json_array_size(numbers));
    
    json_t *first_num = json_array_get(numbers, 0);
    TEST_ASSERT_NOT_NULL(first_num);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, first_num->valuenumber);
    
    json_delete(obj_with_array);
    
    // Array containing objects
    json_t *array_with_objs = json_parse("[{\"name\": \"John\"}, {\"name\": \"Jane\"}]");
    
    TEST_ASSERT_NOT_NULL(array_with_objs);
    TEST_ASSERT_EQUAL(JSON_ARRAY, array_with_objs->type);
    TEST_ASSERT_EQUAL_INT(2, json_array_size(array_with_objs));
    
    json_t *first_obj = json_array_get(array_with_objs, 0);
    TEST_ASSERT_NOT_NULL(first_obj);
    TEST_ASSERT_EQUAL(JSON_OBJECT, first_obj->type);
    
    json_t *john_name = json_object_get(first_obj, "name");
    TEST_ASSERT_NOT_NULL(john_name);
    TEST_ASSERT_EQUAL_STRING("John", john_name->valuestring);
    
    json_t *second_obj = json_array_get(array_with_objs, 1);
    json_t *jane_name = json_object_get(second_obj, "name");
    TEST_ASSERT_NOT_NULL(jane_name);
    TEST_ASSERT_EQUAL_STRING("Jane", jane_name->valuestring);
    
    json_delete(array_with_objs);
}

// Test array error cases
void test_array_error_cases(void) {
    // Missing closing bracket
    TEST_ASSERT_NULL(json_parse("[1, 2, 3"));
    
    // Missing value after comma
    TEST_ASSERT_NULL(json_parse("[1, 2,]"));
    
    // Extra comma
    TEST_ASSERT_NULL(json_parse("[1,, 2]"));
    
    // Invalid value
    TEST_ASSERT_NULL(json_parse("[invalid]"));
}

// Test array access edge cases
void test_array_access_edge_cases(void) {
    json_t *arr = json_parse("[1, 2, 3]");
    
    // Valid access
    TEST_ASSERT_NOT_NULL(json_array_get(arr, 0));
    TEST_ASSERT_NOT_NULL(json_array_get(arr, 2));
    
    // Invalid indices
    TEST_ASSERT_NULL(json_array_get(arr, -1));   // Negative
    TEST_ASSERT_NULL(json_array_get(arr, 3));    // Out of bounds
    TEST_ASSERT_NULL(json_array_get(arr, 100));  // Way out of bounds
    
    // Wrong type
    json_t *obj = json_parse("{}");
    TEST_ASSERT_NULL(json_array_get(obj, 0));    // Not an array
    TEST_ASSERT_EQUAL_INT(0, json_array_size(obj)); // Not an array
    
    json_delete(arr);
    json_delete(obj);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_parse_empty_array);
    RUN_TEST(test_parse_single_element_array);
    RUN_TEST(test_parse_number_array);
    RUN_TEST(test_parse_mixed_array);
    RUN_TEST(test_parse_array_with_whitespace);
    RUN_TEST(test_parse_nested_structures);
    RUN_TEST(test_array_error_cases);
    RUN_TEST(test_array_access_edge_cases);
    
    return UNITY_END();
}