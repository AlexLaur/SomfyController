/**
 * @file Remotes.h
 * @author Laurette Alexandre
 * @brief Header for remotes.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <Vector.h>

/**
 * @brief Remote is a representation of a SOMFY remote.
 *
 */
struct Remote
{
  unsigned long id;
  unsigned int rolling_code;
  char* name;
  bool enabled;
  unsigned short eeprom_address;
};

/**
 * @brief RemoteState is used to store remote infos into the EEPROM.
 *
 */
struct RemoteState
{
  unsigned long id;
  unsigned int rolling_code;
  bool enabled;
};

/**
 * @brief All available actions for remotes
 *
 */
enum REMOTES_ACTIONS
{
  UP,
  DOWN,
  STOP,
  PROG,
  RESET,
  ENABLE,
  DISABLE,
  UNKNOWN,
};

/**
 * @brief Manager for all remotes.
 *
 */
class RemotesManager
{
  static const unsigned long REMOTE_BASE_ADDRESS;
  static const unsigned int MAX_REMOTES;

  public:
  RemotesManager(Vector<Remote> remotes);
  ~RemotesManager();
  void load_remotes(const char* remote_names[], int nb_remotes);

  Vector<Remote> get_remotes();
  Remote* get_remote(unsigned long remote_id);
  void update_remote(Remote remote);

  void increment_rolling_code(Remote remote);
  void increment_rolling_code(unsigned long remote_id);
  void reset_rolling_code(Remote remote);
  void reset_rolling_code(unsigned long remote_id);

  void toggle_remote_enable(Remote remote);
  void toggle_remote_enable(unsigned long remote_id);

  void print_remotes();

  private:
  Vector<Remote> remotes;
  void update(Remote remote); // TODO fix the name
  void get_remotes_states();
};