#pragma once

#include "wifiClient.h"
#include "dto/networks.h"
#include "dto/remote.h"

class Database {
public:
    Database();

    NetworkConfiguration getNetworkConfiguration();
    void getRemotes();
};