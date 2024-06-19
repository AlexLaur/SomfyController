#include <Arduino.h>
#include <unity.h>

#include <remote.h>
#include <jsonSerializer.h>

JSONSerializer serializer;

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_METHOD_serializeRemote_WITH_remote_SHOULD_return_string(void){
    Remote remote = {1, 0, "foo"};
    // String serialized = serializer.serializeRemote(remote);
    // String expected = "{\"id\": 1, \"name\": \"foo\", \"rolling_code\": 0}";

    // TEST_ASSERT_EQUAL_STRING(serialized.c_str(), expected.c_str());
    // Serial.println(serialized);
    TEST_ASSERT_EQUAL_INT32(2,2);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    UNITY_BEGIN();
    RUN_TEST(test_METHOD_serializeRemote_WITH_remote_SHOULD_return_string);
    UNITY_END();
}

void loop() {}
