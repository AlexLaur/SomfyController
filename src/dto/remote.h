#pragma once

#include "../../default_config.h"

struct Remote
{
    unsigned long id;
    unsigned int rollingCode;
    char name[MAX_REMOTE_NAME_LENGTH];
};
