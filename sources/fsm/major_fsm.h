#pragma once
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include <iostream>
#include "boost/variant.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;
struct initial_state;

struct event
{
    virtual ~event() = 0;
};

// events
struct start_music       : public event { virtual ~start_music(){};};
struct greetings         : public event { virtual ~greetings(){};};
struct stop              : public event { virtual ~stop(){};};
struct goodbye           : public event { virtual ~goodbye(){};};
struct finished_music    : public event { virtual ~finished_music(){};};
struct finished_farewell : public event { virtual ~finished_farewell(){};};

// A typedef of boost::variant, put all event here
using EvtType = boost::variant<start_music, greetings, stop, goodbye, finished_music, finished_farewell>;

struct common_fsm;
typedef msm::back::state_machine<common_fsm> common_fsm_backend;

struct common_fsm : public msm::front::state_machine_def<common_fsm>
{
    struct Empty : public msm::front::state<>
    {
        // every (optional) entry/exit methods get the event passed.
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&) {std::cout << "entering: Empty" << std::endl;}
        template <class Event, class FSM>
        void on_exit(Event const&, FSM&) {std::cout << "leaving: Empty" << std::endl;}
    };
    struct Start : public msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&) {std::cout << "entering: Start" << std::endl;}
        template <class Event, class FSM>
        void on_exit(Event const&, FSM& ) {std::cout << "leaving: Start" << std::endl;}
    };

    // sm_ptr still supported but deprecated as functors are a much better way to do the same thing
    struct Stopped : public msm::front::state<msm::front::default_base_state,msm::front::sm_ptr>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Stopped" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Stopped" << std::endl;}
        void set_sm_ptr(common_fsm* cmn_fsm)
        {
            m_common_fsm = cmn_fsm;
        }
        common_fsm* m_common_fsm;
    };

    struct Playing_music : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Playing_music" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Playing_music" << std::endl;}
    };

    struct Greeting : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Greeting" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Greeting" << std::endl;}
    };

    struct Farewell : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Farewell" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Farewell" << std::endl;}
    };

    // state not defining any entry or exit
    struct Paused : public msm::front::state<>
    {
    };

    // template <class Event, class FSM>
    // void on_entry(Event const&, FSM&);
    // template <class Event, class FSM>
    // void on_exit(Event const&, FSM& );

    template <class Event, class FSM>
    void on_entry(Event const&, FSM&)
    {
        std::cout << "entering: common_fsm" << std::endl;
    }

    template <class Event,class FSM>
    void on_exit(Event const&, FSM&)
    {
        std::cout << "leaving: common_fsm" << std::endl;
    }

    // The list of FSM states
    // the initial state of the common_fsm SM. Must be defined
    typedef Start initial_state;

    // transition actions
    void play_music(start_music const&);
    void say_greetings(greetings const&);
    void stop_music(stop const&);
    void say_goodbye(goodbye const&);
    // guard conditions

    // Transition table for main_fsm

    struct transition_table : mpl::vector<
        //    Start            Event                Next               Action				 Guard
        //  +---------------+-------------------+---------------+----------------------------+----------------------+
       _row < Start         , stop              , Start                                                       >,
      a_row < Start         , start_music       , Playing_music , &common_fsm::play_music                     >,
      a_row < Start         , greetings         , Greeting      , &common_fsm::say_greetings                  >,
      a_row < Start         , goodbye           , Farewell      , &common_fsm::say_goodbye                    >,
       //  +----------------+-------------------+---------------+----------------------------+-----------------------+
      _row < Farewell       , finished_farewell , Start                                                       >,
        //  +---------------+-------------------+---------------+----------------------------+-----------------------+
       _row < Playing_music , finished_music    , Start                                                       >,
      a_row < Playing_music , stop              , Start         , &common_fsm::stop_music                     >
        //  +---------------+-------------------+---------------+----------------------------+-----------------------+
    > {};
    // Replaces the default no-transition response.
    template <class FSM,class Event>
    // void no_transition(Event const&, FSM&, int);
    void no_transition(Event const& e, FSM&,int state)
    {
        std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
    }
    static void pstate(common_fsm_backend const& p);
};

