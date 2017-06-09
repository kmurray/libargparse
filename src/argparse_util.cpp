#include "argparse_util.hpp"
#include <cstring>

namespace argparse {

    std::array<std::string,2> split_leading_dashes(std::string str) {
        auto iter = str.begin();
        while(*iter == '-') {
            ++iter;
        }

        std::string dashes(str.begin(), iter);
        std::string name(iter, str.end());
        std::array<std::string,2> array = {dashes, name};

        return array;
    }

    bool is_argument(std::string str) {
        if (str.size() == 2 && str[0] == '-' && str[1] != '-') {
            return true; //Short option
        } else if (str.size() > 2 && str[0] == '-' && str[1] == '-' && str[2] != '-') {
            return true; //Long option
        }
        return false;
    }

    std::string toupper(std::string str) {
        std::string upper;
        for (size_t i = 0; i < str.size(); ++i) {
            char C = ::toupper(str[i]);
            upper.push_back(C);
        }
        return upper;
    }

    std::string tolower(std::string str) {
        std::string lower;
        for (size_t i = 0; i < str.size(); ++i) {
            char C = ::tolower(str[i]);
            lower.push_back(C);
        }
        return lower;
    }

    const char* strdup(const char* str) {
        size_t len = std::strlen(str);
        char* res = new char[len+1]; //+1 for terminator
        std::strcpy(res, str);
        return res;
    }
} //namespace
