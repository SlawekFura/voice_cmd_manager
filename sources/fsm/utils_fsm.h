#pragma once

template <class FSM>
void fsm_process_event(FSM& fsm, EvtType v)
{
    boost::apply_visitor([sm=&fsm](auto const& evt) {
            sm->process_event(evt); }, v);
}