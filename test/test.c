#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "../src/util.h"
#include "../src/parser.c"

const char *test_func_names[] = {
    "test_heading"
}

bool (*test_funcs[])(void) = {
    test_heading,
};

// Tests //

bool test_heading(void)
{
    // TODO: Note string must be null terminated
    const char *test_input = "# This a test for heading\0";

    // TODO:
    // Generate what the output buffer should be
    // Generate the buffer from the test input
    // Compare the buffers to make sure they are the same

    return true;
}

// Test Core // 

bool test_verify_matching_array(token_t *tb_a, token_t *tb_b)
{
    // TODO: Implement
    return true;
}

int main(int argc, char **argv)
{
    uint32_t test_funcs_size = arr_size(test_funcs);
    // Name array has to be same length as the test funcs
    assert(test_funcs_size == arr_size(test_func_name));


    uint32_t tests_passed_count = 0;
    bool (*current)(void);
    for (int i = 0; i < test_funcs_size; ++i)
    {
        current = test_funcs[i];

        printf("%s: ", test_func_names[i]);
        if (current())
        {
            // Test passed
            tests_passed_count++;
            printf("PASSED\n");
        }
        else
        {
            // Test failed
            printf("FAILED\n");
        }
    }

    printf("Test finished running...\n(%d/%d) passed\n", 
        tests_passed_count, test_funcs_size);

    return 0;
}
