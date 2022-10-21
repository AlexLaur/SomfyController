/**
 * @file config.h
 * @author Laurette Alexandre
 * @brief Configuration for SOMFY Controller
 * @version 0.1
 * @date 2022-10-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "remotes.h"

const char SSID[] = "";
const char PASSWORD[] = "";

const int SERVER_PORT = 80;

Remote DEFAULT_REMOTES [] = {
    {0x100000, 0, "remote00", false},
    {0x100001, 0, "remote01", false},
    {0x100002, 0, "remote02", false},
    {0x100003, 0, "remote03", false},
    {0x100004, 0, "remote04", false},
    {0x100005, 0, "remote05", false},
    {0x100006, 0, "remote06", false},
    {0x100007, 0, "remote07", false},
    {0x100008, 0, "remote08", false},
    {0x100009, 0, "remote09", false},
};
