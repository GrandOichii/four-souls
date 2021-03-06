#include "util.hpp"

namespace str {
    vector<string> widthSplit(string text, int maxWidth) {
        vector<string> result;
        auto newLineSplit = split(text, "\n");
        for (auto subText : newLineSplit) {
            auto sp = split(subText, " ");
            auto line = sp[0];
            for (int i = 1; i < sp.size(); i++) {
                auto word = sp[i];
                if ((line + " " + word).length() > maxWidth) {
                    result.push_back(line);
                    line = word;
                }
                else {
                    line += " " + word;
                }
            }
            result.push_back(line);
        }
        return result;
    }
    
    vector<string> split(string s, const string delimiter) {
        size_t pos = 0;
        string token;
        vector<string> result;
        while ((pos = s.find(delimiter)) != string::npos) {
            token = s.substr(0, pos);
            result.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        result.push_back(s);
        return result;
    }
}

namespace fs {
    nlohmann::json readJS(const string path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("path " + string(path) + " doesn't exist");
        nlohmann::json j;
        in >> j;
        in.close();
        return j;
    }

    string join(const vector<string> dirs) {
        return str::join(dirs.begin(), dirs.end(), PATH_DELIMITER);
    }
    
    string join(const string d1, const string d2) {
        return join(vector<string>{d1, d2});
    }

    string dirOf(const string path) {
        string directory;
        const size_t last_slash_idx = path.rfind(PATH_DELIMITER);
        if (std::string::npos != last_slash_idx)
        {
            return path.substr(0, last_slash_idx);
        }
        throw std::runtime_error("can't extract directory from file " + path);
    }

    string readFile(const char* path) {
        string line;
        std::ifstream in(path);
        if (!in) {
            throw std::runtime_error("no file " + string(path));
        }
        vector<string> result;
        while (std::getline(in, line)) {
            result.push_back(line);
        }
        in.close();
        return str::join(result.begin(), result.end(), "\n");
    }
}