#pragma once

#include <string>
#include <iostream>

using std::string;

const string ROLL_TRIGGER = "roll";
const string DEATH_TRIGGER = "death";
const string TURN_START_TRIGGER = "turn_start";

const string PASS_RESPONSE = "$PASS";

struct ActivatedAbility {
    std::vector<string> cost;
    string funcName;

    void print() {
        std::cout << "Cost: " << str::join(cost.begin(), cost.end(), ", ") << std::endl;
        std::cout << "funcName: " << funcName << std::endl;
    }
};
