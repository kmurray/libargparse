#ifndef ARGPARSE_UTIL_HPP
#define ARGPARSE_UTIL_HPP
#include <array>

namespace argparse {

    std::array<std::string,2> split_leading_dashes(std::string str);

    std::string toupper(std::string str);

} //namespace
#endif
