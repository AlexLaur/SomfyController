#include "database.h"
#include "dto/networks.h"
#include "dto/remote.h"

Database::Database() { }

NetworkConfiguration Database::getNetworkConfiguration() {
    NetworkConfiguration conf = {"foo", "bar"};
    return conf;
}

void Database::getRemotes() { }