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
    void load_remotes(const char* remote_names[], int nb_remotes);

    Vector<Remote> get_remotes();
    void update_remote(Remote remote);

    void reset_rolling_code(Remote remote);
    void reset_rolling_code(unsigned long remote_id);

    void toggle_remote_enable(Remote remote);
    void toggle_remote_enable(unsigned long remote_id);

    void print_remotes();

private:
    Vector<Remote> remotes;
    void update(Remote remote); // TODO fix the name
    void update_remotes_from_memory(); // TODO find better name
};