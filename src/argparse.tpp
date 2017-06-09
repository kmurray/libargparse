#include <sstream>
#include "argparse_util.hpp"

namespace argparse {
    
    template<typename T, typename Converter>
    std::shared_ptr<Argument> make_argument(T& dest, std::string long_opt, std::string short_opt) {
        auto ptr = std::make_shared<SingleValueArgument<T, Converter>>(dest, long_opt, short_opt);

        //If the conversion object specifies a non-empty set of choices
        //use those by default
        auto default_choices = Converter().default_choices();
        if (!default_choices.empty()) {
            ptr->choices(default_choices);
        }

        return ptr;
    }

#ifdef MULTI_VALUE
    template<typename T>
    std::shared_ptr<Argument> make_argument(std::vector<T>& dest, std::string long_opt, std::string short_opt) {
        return std::shared_ptr<Argument>(new MultiValueArgument<std::vector<T>>(dest, long_opt, short_opt));
    }
#endif

    /*
     * ArgumentParser
     */

    template<typename T, typename Converter>
    Argument& ArgumentParser::add_argument(T& dest, std::string option) {
        return add_argument<T,Converter>(dest, option, std::string());
    }

    template<typename T, typename Converter>
    Argument& ArgumentParser::add_argument(T& dest, std::string long_opt, std::string short_opt) {
        return argument_groups_[0].add_argument<T,Converter>(dest, long_opt, short_opt);
    }

    /*
     * ArgumentGroup
     */
    template<typename T, typename Converter>
    Argument& ArgumentGroup::add_argument(T& dest, std::string option) {
        return add_argument<T,Converter>(dest, option, std::string());
    }

    template<typename T, typename Converter>
    Argument& ArgumentGroup::add_argument(T& dest, std::string long_opt, std::string short_opt) {
        arguments_.push_back(make_argument<T,Converter>(dest, long_opt, short_opt));
        return *arguments_[arguments_.size() - 1];
    }

    /*
     * Argument
     */

    /*
     * SingleValueArgument
     */
    template<typename T, typename Converter>
    SingleValueArgument<T,Converter>::SingleValueArgument(T& dest, std::string long_opt, std::string short_opt)
        : Argument(long_opt, short_opt)
        , dest_(dest)
        {}

    template<typename T, typename Converter>
    void SingleValueArgument<T,Converter>::set_dest_to_default() {
        set_dest_to_value_from_str(default_value());
    }

    template<typename T, typename Converter>
    void SingleValueArgument<T,Converter>::set_dest_to_value_from_str(std::string val) {
        set_dest_to_value(Converter().from_str(val));
    }

    template<typename T, typename Converter>
    void SingleValueArgument<T,Converter>::set_dest_to_value(T val) {
        dest_ = val;
    }

#ifdef MULTI_VALUE
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

#endif

} //namespace
