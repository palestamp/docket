#include "unity.h"

void test_me(void)
{
	/* All of these should pass */
	TEST_ASSERT_EQUAL(0, 0);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_me);
	UNITY_END();
}


