#pragma once

#include <map>
#include <memory>

#include "major_fsm.h"

// struct event;
// struct greetings;
// struct goodbye;
// struct stop;
// struct start_music;

enum class Intent
{
    Intent_invalid,
    Intent_greetings,
    Intent_goodbye,
    Intent_music,
    Intent_stop,
};

using Event_map = std::map<std::string, std::unique_ptr<EvtType>>;

static Event_map event_mapping_init()
{
  Event_map local_map;
  local_map["invalid"] = std::make_unique<EvtType>(invalid{});
  local_map["powitanie"] = std::make_unique<EvtType>(greetings{});
  local_map["pozegnanie"] = std::make_unique<EvtType>(goodbye{});
  local_map["stop"] = std::make_unique<EvtType>(stop{});
  local_map["muzyka"] = std::make_unique<EvtType>(start_music{});
  local_map["nastepna"] = std::make_unique<EvtType>(next_track{});

  return local_map;
}
extern Event_map intent_event_map;
