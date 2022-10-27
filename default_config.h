/**
 * @file config.h
 * @author Laurette Alexandre
 * @brief Configuration for SOMFY Controller
 * @version 0.1
 * @date 2022-10-20
 *
 * This file should be rename in "config.h".
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

const char SSID[] = "";
const char PASSWORD[] = "";

const int SERVER_PORT = 80;

// Remotes. You can change the name of remotes here.
const char* SOMFY_CONFIG_REMOTES[] = {
    "Remote 00",
    "Remote 01",
    "Remote 02",
    "Remote 03",
    "Remote 04"
};
