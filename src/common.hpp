#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <time.h>

#include "game/match.hpp"

#include "network/olc_network.hpp"

#include "util.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;


enum PollType : uint8_t {
    // client - server
    ServerMessage,

    // server - client
    Setup,
    UpdateWinner,
    Update,
    GetAction,
    Prompt,
    SimplePrompt,
    ChooseCards
};