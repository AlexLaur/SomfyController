/**
 * @file default_config.h
 * @author Laurette Alexandre
 * @brief Configuration for SOMFY Controller
 * @version 2.0.0
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

const char AP_SSID[] = "SomfyController Fallback Hotspot";
const char AP_PASSWORD[] = "5cKErSRCyQzy";

const int SERVER_PORT = 80;

const unsigned short MAX_NETWORK_SCAN = 15;

const unsigned short MAX_REMOTES = 32;
const unsigned long REMOTE_BASE_ADDRESS = 0x100000;