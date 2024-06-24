#include <Arduino.h>
#include <unity.h>

#include <remote.h>
#include <RTSTransmitter.h>
#include "./test_RTSTransmitter.h"

RTSTransmitter transmitterTest;

void RUN_RTSTRANSMITTER_TESTS(void){
    RUN_TEST(test_METHOD_sendUpCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame);
    RUN_TEST(test_METHOD_sendStopCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame);
    RUN_TEST(test_METHOD_sendDownCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame);
    RUN_TEST(test_METHOD_sendProgCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame);
}

void compareFramesArray(byte expected[], byte actual[], int size){
    for (int i = 0; i < size; i++)
    {
        TEST_ASSERT_EQUAL(expected[i], actual[i]);
    }
}

void test_METHOD_sendUpCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame(void){
    Remote remoteA = {1048576, 0, "foo"};
    bool resultA = transmitterTest.sendUpCmd(remoteA.id, remoteA.rollingCode);

    TEST_ASSERT_TRUE(resultA);

    byte* buildedFramesAPtr = transmitterTest.getBytesFrame();
    byte expectedFramesA[] = {0xA7, 0x89, 0x89, 0x89, 0x99, 0x99, 0x99};
    compareFramesArray(expectedFramesA, buildedFramesAPtr, 7);

    Remote remoteB = {1048579, 0, "foo"};
    bool resultB = transmitterTest.sendUpCmd(remoteB.id, remoteB.rollingCode);

    TEST_ASSERT_TRUE(resultB);

    byte* buildedFramesBPtr = transmitterTest.getBytesFrame();
    byte expectedFramesB[] = {0xA7, 0x8A, 0x8A, 0x8A, 0x9A, 0x9A, 0x99};
    compareFramesArray(expectedFramesB, buildedFramesBPtr, 7);
}

void test_METHOD_sendStopCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame(void){
    Remote remoteA = {1048576, 1, "foo"};
    bool resultA = transmitterTest.sendStopCmd(remoteA.id, remoteA.rollingCode);

    TEST_ASSERT_TRUE(resultA);

    byte* buildedFramesAPtr = transmitterTest.getBytesFrame();
    byte expectedFramesA[] = {0xA7, 0xBB, 0xBB, 0xBA, 0xAA, 0xAA, 0xAA};
    compareFramesArray(expectedFramesA, buildedFramesAPtr, 7);

    Remote remoteB = {1048579, 1, "foo"};
    bool resultB = transmitterTest.sendStopCmd(remoteB.id, remoteB.rollingCode);

    TEST_ASSERT_TRUE(resultB);

    byte* buildedFramesBPtr = transmitterTest.getBytesFrame();
    byte expectedFramesB[] = {0xA7, 0xB8, 0xB8, 0xB9, 0xA9, 0xA9, 0xAA};
    compareFramesArray(expectedFramesB, buildedFramesBPtr, 7);
}

void test_METHOD_sendDownCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame(void){
    Remote remoteA = {1048576, 2, "foo"};
    bool resultA = transmitterTest.sendDownCmd(remoteA.id, remoteA.rollingCode);

    TEST_ASSERT_TRUE(resultA);

    byte* buildedFramesAPtr = transmitterTest.getBytesFrame();
    byte expectedFramesA[] = {0xA7, 0xED, 0xED, 0xEF, 0xFF, 0xFF, 0xFF};
    compareFramesArray(expectedFramesA, buildedFramesAPtr, 7);

    Remote remoteB = {1048579, 2, "foo"};
    bool resultB = transmitterTest.sendDownCmd(remoteB.id, remoteB.rollingCode);

    TEST_ASSERT_TRUE(resultB);

    byte* buildedFramesBPtr = transmitterTest.getBytesFrame();
    byte expectedFramesB[] = {0xA7, 0xEE, 0xEE, 0xEC, 0xFC, 0xFC, 0xFF};
    compareFramesArray(expectedFramesB, buildedFramesBPtr, 7);
}

void test_METHOD_sendProgCommand_WITH_remote_SHOULD_return_true_AND_build_specific_frame(void){
    Remote remoteA = {1048576, 3, "foo"};
    bool resultA = transmitterTest.sendProgCmd(remoteA.id, remoteA.rollingCode);

    TEST_ASSERT_TRUE(resultA);

    byte* buildedFramesAPtr = transmitterTest.getBytesFrame();
    byte expectedFramesA[] = {0xA7, 0x20, 0x20, 0x23, 0x33, 0x33, 0x33};
    compareFramesArray(expectedFramesA, buildedFramesAPtr, 7);

    Remote remoteB = {1048579, 3, "foo"};
    bool resultB = transmitterTest.sendProgCmd(remoteB.id, remoteB.rollingCode);

    TEST_ASSERT_TRUE(resultB);

    byte* buildedFramesBPtr = transmitterTest.getBytesFrame();
    byte expectedFramesB[] = {0xA7, 0x23, 0x23, 0x20, 0x30, 0x30, 0x33};
    compareFramesArray(expectedFramesB, buildedFramesBPtr, 7);
}