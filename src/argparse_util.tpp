#include <sstream>
#include "argparse.hpp"

namespace argparse {

    template<typename T> 
    T as(std::string str) {
        
        std::stringstream ss(str);

        T val;
        ss >> val;

        if (!ss.good()) {
            std::stringstream msg;
            msg << "Failed to convert value '" << str << "'";
            throw ArgParseError(msg.str().c_str());
        }
    }

}
