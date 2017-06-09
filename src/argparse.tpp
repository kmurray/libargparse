#include <sstream>
#include "argparse_util.hpp"

namespace argparse {
    
    template<typename T>
    std::unique_ptr<Argument> make_argument(T& dest, std::string long_opt, std::string short_opt) {
        return std::unique_ptr<Argument>(new SingleValueArgument<T>(dest, long_opt, short_opt));
    }

    template<typename T>
    std::unique_ptr<Argument> make_argument(std::vector<T>& dest, std::string long_opt, std::string short_opt) {
        return std::unique_ptr<Argument>(new MultiValueArgument<std::vector<T>>(dest, long_opt, short_opt));
    }

    /*
     * ArgumentParser
     */

    template<typename T>
    Argument& ArgumentParser::add_argument(T& dest, std::string option) {
        return add_argument(dest, option, std::string());
    }

    template<typename T>
    Argument& ArgumentParser::add_argument(T& dest, std::string long_opt, std::string short_opt) {
        return argument_groups_[0].add_argument(dest, long_opt, short_opt);
    }

    /*
     * ArgumentGroup
     */
    template<typename T>
    Argument& ArgumentGroup::add_argument(T& dest, std::string option) {
        return add_argument(dest, option, std::string());
    }

    template<typename T>
    Argument& ArgumentGroup::add_argument(T& dest, std::string long_opt, std::string short_opt) {
        arguments_.push_back(make_argument(dest, long_opt, short_opt));
        return *arguments_[arguments_.size() - 1];
    }

    /*
     * Argument
     */
    template<typename T>
    Argument& Argument::default_value(const T& default_val) {
        //Dispatch as appropriate
        SingleValueArgument<T>* single_arg = dynamic_cast<SingleValueArgument<T>*>(this);
        MultiValueArgument<T>* multi_arg = dynamic_cast<MultiValueArgument<T>*>(this);

        if (single_arg) {
            single_arg->default_value(default_val);
        } else if (multi_arg) {
            std::vector<T> default_vals = { default_val};
            multi_arg->default_value(default_vals);
        } else {
            throw ArgParseError("Failed to dispatch default value");
        }
        return *this;
    }

    template<typename T>
    Argument& Argument::default_value(const std::vector<T>& default_vals) {
        //Dispatch to MultiValueArgument
        MultiValueArgument<T>* multi_arg = dynamic_cast<MultiValueArgument<T>>(this);

        if (multi_arg) {
            multi_arg->default_value(default_vals);
        } else {
            throw ArgParseError("Multiple default values can only be assigned to non-scalar destinations.");
        }
        return *this;
    }

    /*
     * SingleValueArgument
     */
    template<typename T>
    SingleValueArgument<T>::SingleValueArgument(T& dest, std::string long_opt, std::string short_opt)
        : Argument(long_opt, short_opt)
        , dest_(dest)
        {}

    template<typename T>
    SingleValueArgument<T>& SingleValueArgument<T>::default_value(T default_val) {
        default_value_ = default_val;
        return *this;
    }

    template<typename T>
    void SingleValueArgument<T>::set_dest_to_value(T val) {
        dest_ = val;
    }

    template<typename T>
    void SingleValueArgument<T>::set_dest_to_default() {
        set_dest_to_value(default_value_);
    }

    template<typename T>
    std::string SingleValueArgument<T>::default_value() const {
        std::stringstream ss;
        ss << default_value_;

        if (!ss.good()) {
            throw ArgParseError("Failed to convert default value to string");
        }

        return ss.str();
    }

    /*
     * MultiValueArgument
     */
    template<typename T>
    MultiValueArgument<T>::MultiValueArgument(std::vector<T>& dest, std::string long_opt, std::string short_opt)
        : Argument(long_opt, short_opt)
        , dest_(dest)
        {}

    template<typename T>
    MultiValueArgument<T>& MultiValueArgument<T>::default_value(std::vector<T> default_vals) {
        default_values_ = default_vals;
        set_dest_to_value(default_vals);
        return *this;
    }

    template<typename T>
    void MultiValueArgument<T>::set_dest_to_value(std::vector<T> val) {
        dest_ = val;
    }

    template<typename T>
    void MultiValueArgument<T>::set_dest_to_default() {
        set_dest_to_value(default_values_);
    }

    template<typename T>
    std::string MultiValueArgument<T>::default_value() const {
        std::stringstream ss;
        ss << "[";

        bool first = true;
        for (T& val : default_values_) {
            if (!first) {
                ss << ", ";
            }
            ss << val;
            first = false;
        }
        ss << "]";

        if (!ss.good()) {
            throw ArgParseError("Failed to convert default values to string");
        }

        return ss.str();
    }

} //namespace
