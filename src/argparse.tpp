#include <sstream>
#include "argparse_util.hpp"

namespace argparse {

    /*
     * Argument
     */
    template<typename T>
    Argument& Argument::default_value(T default_val) {
        if(!default_values_.empty()) {
            throw ArgParseError("Multiple default value specifications");
        }

        std::stringstream ss;
        ss << default_val;

        if (!ss.good()) {
            std::stringstream msg;
            msg << "Failed to convert default value '" << default_val << "' to string";
            throw ArgParseError(msg.str().c_str());
        }

        default_values_.push_back(ss.str());
        return *this;
    }

    template<typename T>
    Argument& Argument::default_value(std::vector<T> default_vals) {
        if(!default_values_.empty()) {
            throw ArgParseError("Multiple default value specifications");
        }

        for(auto& val : default_vals) {
            std::stringstream ss;
            ss << val;

            if (!ss.good()) {
                std::stringstream msg;
                msg << "Failed to convert default value '" << val << "' to string";
                throw ArgParseError(msg.str().c_str());
            }

            default_values_.push_back(ss.str());
        }
        return *this;
    }

    /*
     * ArgValues
     */

    template<class T>
    T ArgValues::value_as(std::string key) const {
        auto range = values_.equal_range(key);
        size_t cnt = std::distance(range.first, range.second);
        if (cnt == 0) {
            std::stringstream msg;
            msg << "Invalid argument name '" << key << "'";
            throw ArgParseError(msg.str().c_str());
        } else if (cnt != 1) {
            std::stringstream msg;
            msg << "There are multiple values for argument name '" << key << "'";
            throw ArgParseError(msg.str().c_str());
        }

        auto iter = range.first;

        return as<T>(iter->second);
    }

    template<class T>
    std::vector<T> ArgValues::values_as(std::string key) const {
        auto range = values_.equal_range(key);
        size_t cnt = std::distance(range.first, range.second);
        if (cnt == 0) {
            std::stringstream msg;
            msg << "Invalid argument name '" << key << "'";
            throw ArgParseError(msg.str().c_str());
        }

        std::vector<T> values;
        for(auto iter = range.first; iter != range.second; ++iter) {
            values.push_back(as<T>(iter->second));
        }

        return values;
    }

}
