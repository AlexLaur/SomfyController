#include <Arduino.h>
#include <unity.h>

#include "./test_jsonSerializer.h"
#include "./test_eepromDatabase.h"
#include "./test_controller.h"

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
  FakeDatabase::shouldFailDeleteRemote = false;
  FakeDatabase::shouldFailUpdateRemote = false;
  FakeDatabase::shouldReturnEmptyRemote = false;
  FakeDatabase::shouldFailCreateRemote = false;
  FakeDatabase::shouldFailUpdateNetworkConfiguration = false;

  FakeTransmitter::sendUPCommandCalled = false;
  FakeTransmitter::sendSTOPCommandCalled = false;
  FakeTransmitter::sendDOWNCommandCalled = false;
  FakeTransmitter::sendPROGCommandCalled = false;
}

void RUN_UNITY_TESTS()
{
  UNITY_BEGIN();
  // JSONSerializer tests
  RUN_TEST(test_METHOD_serializeRemote_WITH_remote_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeRemotes_WITH_two_remotes_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeRemotes_WITH_one_remote_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworkConfig_WITH_config_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeSystemInfos_WITH_info_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworks_WITH_one_network_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworks_WITH_two_networks_SHOULD_return_string);
  // EEPROMDatabase tests

  // Controller tests
  RUN_TEST(test_METHOD_fetchSystemInfos_SHOULD_return_systeminfos);
  RUN_TEST(test_METHOD_fetchRemote_WITH_unspecified_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_fetchRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_fetchRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_createRemote_WITH_null_name_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_empty_name_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_name_too_long_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_deleteRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_deleteRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_deleteRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_updateRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_updateRemote_WITH_valid_remote_AND_name_too_long_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_updateRemote_WITH_valid_remote_AND_null_name_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateRemote_WITH_valid_remote_AND_valid_name_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateRemote_WITH_valid_remote_AND_rolling_code_provided_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateRemote_WITH_valid_remote_AND_valid_data_AND_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_null_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_empty_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_not_found_remote_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_unknown_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_valide_remote_AND_up_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_operateRemote_WITH_valide_remote_AND_stop_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_operateRemote_WITH_valide_remote_AND_down_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_operateRemote_WITH_valide_remote_AND_pair_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_operateRemote_WITH_valide_remote_AND_reset_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_fetchNetworkConfiguration_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateNetworkConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateNetworkConfiguration_WITH_valid_data_AND_empty_password_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_updateNetworkConfiguration_WITH_null_SSID_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_updateNetworkConfiguration_WITH_empty_SSID_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_updateNetworkConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false);
  UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);

  RUN_UNITY_TESTS();
}

void loop() { }

#else

int main(int argc, char** argv)
{
  RUN_UNITY_TESTS();
  return 0;
}

#endif
