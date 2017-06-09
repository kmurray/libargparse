#ifndef ARGPARSE_DEFAULT_CONVERTER_HPP
#define ARGPARSE_DEFAULT_CONVERTER_HPP
#include <sstream>
#include <vector>
#include "argparse_error.hpp"
#include "argparse_util.hpp"

namespace argparse {

template<typename T>
class DefaultConverter {
    public:
        T from_str(std::string str) {
            std::stringstream ss(str);

            T val;
            ss >> val;

            bool eof = ss.eof();
            bool fail = ss.fail();
            bool converted_ok = eof && !fail;
            if (!converted_ok) {
                throw ArgParseError("Invalid conversion from string");
            }

            return val;
        }

        std::string to_str(T val) {
            std::stringstream ss;
            ss << val;

            bool converted_ok = ss.eof() && !ss.fail();
            if (!converted_ok) {
                throw ArgParseError("Invalid conversion to string");
            }
            return ss.str();
        }
        std::vector<std::string> default_choices() { return {}; }
};

//DefaultConverter specializations for bool
// By default std::stringstream doesn't accept "true" or "false"
// as boolean values.
template<>
class DefaultConverter<bool> {
    public:
        bool from_str(std::string str) {
            str = tolower(str);
            if      (str == "0" || str == "false") return false; 
            else if (str == "1" || str == "true")  return true;

            std::string msg = "Unexpected value '" + str + "' (expected one of: " + join(default_choices(), ", ") + ")";
            throw ArgParseConversionError(msg);
        }

        std::string to_str(bool val) {
            if (val) return "true";
            else     return "false";
        }

        std::vector<std::string> default_choices() {
            return {"true", "false"};
        }
};

//DefaultConverter specializations for std::string
// The default conversion checks for eof() which is not set for empty strings
template<>
class DefaultConverter<std::string> {
    public:
        std::string from_str(std::string str) { return str; }
        std::string to_str(std::string val) { return val; }
        std::vector<std::string> default_choices() { return {}; }
};

//DefaultConverter specializations for char*
//  This allocates memory that the user is responsible for freeing
template<>
class DefaultConverter<const char*> {
    public:
        const char* from_str(std::string str) { 
            return strdup(str.c_str()); 
        }
        std::string to_str(const char* val) { return val; }
        std::vector<std::string> default_choices() { return {}; }
};

} //namespace

#endif
