#pragma once

struct Remote
{
    unsigned long remote_id;
    unsigned int rolling_code;
    char* name;
    bool enabled;
    unsigned short eeprom_address;
};
