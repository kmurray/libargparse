#include "argparse_util.hpp"

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

    std::string toupper(std::string str) {
        std::string upper;
        for (size_t i = 0; i < str.size(); ++i) {
            char C = ::toupper(str[i]);
            upper.push_back(C);
        }
        return upper;
    }

} //namespace
