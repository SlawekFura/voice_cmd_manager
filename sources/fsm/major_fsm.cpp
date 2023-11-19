#include <iostream>

#include "major_fsm.h"

#include "audio_mgr.h"

event::~event(){}

// The list of FSM states
// the initial state of the player SM. Must be defined

// transition actions
void common_fsm::play_music(start_music const&)
{
    std::cout << "common_fsm::play_music - start_music" << std::endl;
    audio_manager.play_music_file();
}
void common_fsm::play_next_track(next_track const&)
{
    std::cout << "common_fsm::play_music - next_track" << std::endl;
    audio_manager.stop_music_file();
    audio_manager.play_music_file();
}
void common_fsm::say_greetings(greetings const&) { std::cout << "common_fsm::say_greetings" << std::endl; }
void common_fsm::stop_music(stop const&)
{
    std::cout << "common_fsm::stop_music" << std::endl;
    audio_manager.reset_track_queue();
    audio_manager.stop_music_file();
}
void common_fsm::say_goodbye(goodbye const&) { std::cout << "common_fsm::say_goodbye" << std::endl; }


// template <class FSM,class Event>
// void common_fsm::no_transition(Event const& e, FSM&,int state)
// {
//     std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
// }
//
// Testing utilities.
//
// print state - first "Start" column in transition table, then "Next" column
static char const* const state_names[] = { "Start", "Farewell", "Playing_music", "Greeting"};
void common_fsm::pstate(common_fsm_backend const& p)
{
    std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
}

