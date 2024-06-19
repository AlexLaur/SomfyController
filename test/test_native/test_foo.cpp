#include <unity.h>

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_foo(void)
{
  char* expected = "foo";
  char* src = "foo";
  TEST_ASSERT_EQUAL_STRING(src, expected);
}

int main(int argc, char** argv)
{
  UNITY_BEGIN();
  RUN_TEST(test_foo);
  UNITY_END();
}

void loop() { }
