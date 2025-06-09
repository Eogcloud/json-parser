// tests/test_basic.c
#include "unity/unity.h"
#include "../include/json.h"
#include <string.h>

// Unity setup/teardown
void setUp(void) {
    // Called before each test
}

void tearDown(void) {
    // Called after each test
}

// Test parsing simple string
void test_parse_string(void) {
    json_t *result = json_parse("\"hello world\"");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_STRING, result->type);
    TEST_ASSERT_EQUAL_STRING("hello world", result->valuestring);
    
    json_delete(result);
}

// Test parsing numbers
void test_parse_positive_integer(void) {
    json_t *result = json_parse("42");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_NUMBER, result->type);
    TEST_ASSERT_EQUAL_DOUBLE(42.0, result->valuenumber);
    
    // Alternative: check as integer
    TEST_ASSERT_EQUAL_INT(42, (int)result->valuenumber);
    
    json_delete(result);
}

void test_parse_negative_integer(void) {
    json_t *result = json_parse("-123");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_NUMBER, result->type);
    TEST_ASSERT_EQUAL_DOUBLE(-123.0, result->valuenumber);
    
    json_delete(result);
}

void test_parse_decimal(void) {
    json_t *result = json_parse("3.14");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_NUMBER, result->type);
    TEST_ASSERT_EQUAL_DOUBLE(3.14, result->valuenumber);
    
    json_delete(result);
}

// Test parsing literals
void test_parse_true(void) {
    json_t *result = json_parse("true");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_TRUE, result->type);
    
    json_delete(result);
}

void test_parse_false(void) {
    json_t *result = json_parse("false");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_FALSE, result->type);
    
    json_delete(result);
}

void test_parse_null(void) {
    json_t *result = json_parse("null");
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(JSON_NULL, result->type);
    
    json_delete(result);
}

// Test error cases
void test_parse_empty_string(void) {
    json_t *result = json_parse("");
    TEST_ASSERT_NULL(result);
}

void test_parse_null_input(void) {
    json_t *result = json_parse(NULL);
    TEST_ASSERT_NULL(result);
}

void test_parse_invalid_json(void) {
    json_t *result = json_parse("invalid");
    TEST_ASSERT_NULL(result);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // String tests
    printf("Running string test...\n");
    RUN_TEST(test_parse_string);
    printf("String test passed!\n");
    
    // Number tests
    printf("Running number tests...\n");
    RUN_TEST(test_parse_positive_integer);
    printf("Positive integer passed!\n");
    
    RUN_TEST(test_parse_negative_integer);
    printf("Negative integer passed!\n");
    
    RUN_TEST(test_parse_decimal);
    printf("Decimal passed!\n");
    
    // Literal tests  
    printf("Running literal tests...\n");
    RUN_TEST(test_parse_true);
    RUN_TEST(test_parse_false);
    RUN_TEST(test_parse_null);
    
    // Error handling tests
    printf("Running error tests...\n");
    RUN_TEST(test_parse_empty_string);
    RUN_TEST(test_parse_null_input);
    RUN_TEST(test_parse_invalid_json);
    
    return UNITY_END();
}