#include <sstream>
namespace argparse {

    /*
     * Argument
     */
    template<class T>
    Argument& Argument::default_value(T default_val) {
        std::stringstream ss;
        ss << default_val;

        if (!ss.good()) {
            throw ArgParseError("Failed to convert default value to string");
        }

        default_value_ = ss.str();
        return *this;
    }


}
