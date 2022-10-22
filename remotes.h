#pragma once

#include <Vector.h>

struct Remote
{
    unsigned long id;
    unsigned int rolling_code;
    char* name;
    bool enabled;
    unsigned short eeprom_address;
};

class RemotesContainer
{
    static const unsigned long REMOTE_BASE_ADDRESS;
public:
    RemotesContainer(Vector<Remote> remotes);
    ~RemotesContainer();
    void load_remotes(const char * remote_names[], int nb_remotes);
    Vector<Remote> get_remotes();
    void update_remote(Remote remote);
    void reset_rolling_code(Remote remote);
    void print_remotes();
private:
    Vector<Remote> remotes_;
    void update_remotes_from_memory(); // TODO find better name
};