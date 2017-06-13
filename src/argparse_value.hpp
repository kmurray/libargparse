#ifndef ARGPARSE_VALUE_HPP
#define ARGPARSE_VALUE_HPP
#include <iostream>

namespace argparse {

    //How the value associated with an argumetn was initialized
    enum class Provenance {
        UNSPECIFIED,//The value was default constructed
        DEFAULT,    //The value was set by a default (e.g. as a command-line argument default value)
        SPECIFIED   //The value was explicitly specified (e.g. explicitly specified on the command-line)
    };

    /*
     * ArgValue represents the 'value' of a command-line option/argument
     *
     * It supports implicit conversion to the underlying value_type, which means it can
     * be seamlessly used as the value_type in most situations.
     *
     * It additionally tracks the provenance off the option, along with it's associated argument group.
     */
    template<typename T>
    class ArgValue {
        public:
            typedef T value_type;

            ArgValue()
                : value_(T())
                , provenance_(Provenance::UNSPECIFIED)
                , group_("")
                {}

        public: //Accessors
            //Automatic conversion to underlying value type
            operator T() const { return value_; }

            //Returns the value assoicated with this argument
            const T& value() const { return value_; }

            //Returns the provenance of this argument (i.e. how it was initialized)
            Provenance provenance() const { return provenance_; }

            //Returns the group this argument is associated with (or an empty string if none)
            const std::string& group() const { return group_; }

        public: //Mutators
            void set(T val, Provenance prov, std::string grp="") {
                value_ = val;
                provenance_ = prov;
                group_ = grp;
            }
        private:
            T value_;
            Provenance provenance_;
            std::string group_;
    };

    //Automatically convert to the underlying type for ostream output
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const ArgValue<T> t) {
        return os << T(t);
    }

}
#endif
