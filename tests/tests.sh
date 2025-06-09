#!/bin/bash

# test.sh - Comprehensive JSON parser test script

set -e  # Exit on any error

# Detect if we're in tests directory and adjust paths
if [[ $(basename $(pwd)) == "tests" ]]; then
    echo "Running from tests directory, adjusting paths..."
    cd ..
fi

# Verify we're in the right place
if [[ ! -f "Makefile" ]] || [[ ! -f "include/json.h" ]]; then
    echo "Error: Please run this script from the project root directory"
    echo "Current directory: $(pwd)"
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Function to test JSON string
test_json() {
    local json="$1"
    local description="$2"
    local should_pass="$3"  # true/false
    
    echo -n "Testing $description... "
    
    if echo "$json" | ./build/json-parser -v - >/dev/null 2>&1; then
        if [ "$should_pass" = "true" ]; then
            echo -e "${GREEN}PASS${NC}"
            return 0
        else
            echo -e "${RED}FAIL${NC} (should have failed but didn't)"
            return 1
        fi
    else
        if [ "$should_pass" = "false" ]; then
            echo -e "${GREEN}PASS${NC} (correctly rejected)"
            return 0
        else
            echo -e "${RED}FAIL${NC} (should have passed but didn't)"
            return 1
        fi
    fi
}

# Main script
echo "=================================================="
echo "         JSON Parser Comprehensive Test"
echo "=================================================="
echo

# Clean and build
print_status "Cleaning and building..."
make clean >/dev/null 2>&1
make debug >/dev/null 2>&1
make json-parser >/dev/null 2>&1

print_success "Build complete"
echo

# Run unit tests
print_status "Running unit tests..."
echo

echo "=== Basic Tests ==="
if make test >/dev/null 2>&1; then
    print_success "Basic tests passed"
else
    print_error "Basic tests failed"
    exit 1
fi

echo "=== Object Tests ==="
if make test-objects >/dev/null 2>&1; then
    print_success "Object tests passed"
else
    print_error "Object tests failed"
    exit 1
fi

echo "=== Array Tests ==="
if make test-arrays >/dev/null 2>&1; then
    print_success "Array tests passed"
else
    print_error "Array tests failed"
    exit 1
fi

echo
print_success "All unit tests passed!"
echo

# Integration tests with real JSON
print_status "Running integration tests with real JSON..."
echo

passed=0
total=0

# Basic values
test_json '"hello world"' "simple string" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '42' "positive integer" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '-123' "negative integer" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '3.14159' "decimal number" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json 'true' "boolean true" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json 'false' "boolean false" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json 'null' "null value" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Objects
test_json '{}' "empty object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"name": "John"}' "simple object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"name": "John", "age": 30}' "multi-property object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"name": "John", "age": 30, "active": true, "score": null}' "complex object" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Arrays
test_json '[]' "empty array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[1]' "single element array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[1, 2, 3, 4, 5]' "number array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '["hello", "world"]' "string array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[1, "hello", true, null, false]' "mixed type array" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Nested structures
test_json '{"users": ["John", "Jane"]}' "object with array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[{"name": "John"}, {"name": "Jane"}]' "array of objects" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"data": {"users": [{"name": "John", "active": true}]}}' "deeply nested" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Real-world-ish examples
test_json '{"id": 1, "name": "John Doe", "email": "john@example.com", "age": 30, "active": true}' "user object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"products": [{"id": 1, "name": "Widget", "price": 19.99}, {"id": 2, "name": "Gadget", "price": 29.99}]}' "product catalog" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"config": {"debug": true, "timeout": 5000, "servers": ["srv1", "srv2", "srv3"]}}' "configuration object" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Error cases - these should fail
print_status "Testing error cases (should fail)..."
test_json '' "empty string" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json 'invalid' "invalid literal" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"key": }' "missing value" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"key" "value"}' "missing colon" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"key": "value",}' "trailing comma in object" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[1, 2,]' "trailing comma in array" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{"key": "unclosed string}' "unclosed string" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{unclosed object' "unclosed object" false; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[unclosed array' "unclosed array" false; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Whitespace tolerance
print_status "Testing whitespace tolerance..."
test_json ' { "key" : "value" } ' "spaced object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json ' [ 1 , 2 , 3 ] ' "spaced array" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json $'{\n  "name": "John",\n  "age": 30\n}' "multiline object" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo

# Edge cases
print_status "Testing edge cases..."
test_json '0' "zero" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '-0' "negative zero" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '""' "empty string" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '{}' "empty object" true; ((total++)); [ $? -eq 0 ] && ((passed++))
test_json '[]' "empty array" true; ((total++)); [ $? -eq 0 ] && ((passed++))

echo
echo "=================================================="
echo "                 TEST SUMMARY"
echo "=================================================="
echo

if [ $passed -eq $total ]; then
    print_success "All tests passed! ($passed/$total)"
    echo
    print_success "🎉 JSON parser is working perfectly!"
    echo
    
    # Show some example usage
    print_status "Example usage:"
    echo './build/json-parser file.json'
    echo 'echo \'{"test": "data"}\' | ./build/json-parser -'
    echo './build/json-parser -v file.json  # validation only'
    
    exit 0
else
    print_error "Some tests failed: $passed/$total passed"
    exit 1
fi