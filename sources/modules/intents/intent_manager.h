#pragma once

#include "types.h"
#include "major_fsm.h"

std::unique_ptr<EvtType>& dispatch_intent(std::string);
