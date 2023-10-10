#include <iostream>

#include "major_fsm.h"

event::~event(){}

// front-end: define the FSM structure
// template <class Event, class FSM>
// void common_fsm::on_entry(Event const&, FSM&)
// {
//     std::cout << "entering: common_fsm" << std::endl;
// }

// template <class Event,class FSM>
// void common_fsm::on_exit(Event const&, FSM&)
// {
//     std::cout << "leaving: common_fsm" << std::endl;
// }

// The list of FSM states
// the initial state of the player SM. Must be defined

// transition actions
void common_fsm::play_music(start_music const&) { std::cout << "common_fsm::start_playback" << std::endl; }
void common_fsm::say_greetings(greetings const&) { std::cout << "common_fsm::say_greetings" << std::endl; }
void common_fsm::stop_music(stop const&) { std::cout << "common_fsm::stop_music" << std::endl; }
void common_fsm::say_goodbye(goodbye const&) { std::cout << "common_fsm::say_goodbye" << std::endl; }


// template <class FSM,class Event>
// void common_fsm::no_transition(Event const& e, FSM&,int state)
// {
//     std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
// }
//
// Testing utilities.
//
static char const* const state_names[] = { "Start", "Playing_music", "Greeting", "Farewell" };
void common_fsm::pstate(common_fsm_backend const& p)
{
    std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
}

