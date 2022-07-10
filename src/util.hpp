#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "nlohmann/json.hpp"

using std::string;
using std::vector;
using json = nlohmann::json;

#define PATH_DELIMITER "/"

namespace str {
    template <class InputIt>
    string join(InputIt begin, const InputIt end, const string delimiter) {
        string result = "";
        while (begin != end-1) {
            result += *begin++ + delimiter;
        }
        result += *begin;
        return result;
    }

    vector<string> split(string s, const string delimiter);
}

namespace fs {
    nlohmann::json readJS(const string path);
    string join(const vector<string> dirs);
    string join(const string d1, const string d2);
    string dirOf(const string path);
    string readFile(const char* path);
}
