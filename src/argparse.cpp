#include <algorithm>
#include <array>
#include <list>
#include <cassert>
#include <string>

#include "argparse.hpp"
#include "argparse_util.hpp"

namespace argparse {

    /*
     * ArgumentParser
     */

    ArgumentParser::ArgumentParser(std::string description_str, std::ostream& os)
        : description_(description_str)
        , formatter_(new DefaultFormatter())
        , os_(os)
        {
        argument_groups_.push_back(ArgumentGroup("arguments:"));
    }

    ArgumentParser& ArgumentParser::prog(std::string prog_name) {
        prog_ = prog_name;
        return *this;
    }

    ArgumentParser& ArgumentParser::epilog(std::string epilog_str) {
        epilog_ = epilog_str;
        return *this;
    }

    ArgumentGroup& ArgumentParser::add_argument_group(std::string description_str) {
        argument_groups_.push_back(ArgumentGroup(description_str));
        return argument_groups_[argument_groups_.size() - 1];
    }


    Argument& ArgumentParser::add_argument(std::string option) {
        return add_argument(option, std::string());
    }

    Argument& ArgumentParser::add_argument(std::string long_opt, std::string short_opt) {
        return argument_groups_[0].add_argument(long_opt, short_opt);
    }

    ArgValues ArgumentParser::parse_args(int argc, const char** argv) {
        std::vector<std::string> arg_strs;
        for (int i = 1; i < argc; ++i) {
            arg_strs.push_back(argv[i]);
        }
        return parse_args(arg_strs);
    }
    
    ArgValues ArgumentParser::parse_args(std::vector<std::string> arg_strs) {
        ArgValues arg_values;
        //Set any default values
        for (const auto& group : argument_groups()) {
            for (const auto& arg : group.arguments()) {
                if (arg.default_value() != "") {
                    arg_values.set(arg.dest(), arg.default_value());
                }
            }
        }


        //Create a look-up of expected argument strings and positional arguments
        std::map<std::string,const Argument&> str_to_option_arg;
        std::list<Argument> positional_args;
        for (const auto& group : argument_groups()) {
            for (const auto& arg : group.arguments()) {
                if (arg.positional()) {
                    positional_args.push_back(arg);
                } else {
                    for (const auto& opt : {arg.long_option(), arg.short_option()}) {
                        if (opt.empty()) continue;

                        auto ret = str_to_option_arg.insert(std::make_pair(opt, arg));

                        if (!ret.second) {
                            //Option string already specified
                            std::stringstream ss;
                            ss << "Option string '" << opt << "' maps to multiple options";
                            throw ArgParseError(ss.str().c_str());
                        }
                    }
                }
            }
        }


        //Process the arguments
        for (size_t i = 0; i < arg_strs.size(); i++) {
            auto iter = str_to_option_arg.find(arg_strs[i]);
            if (iter != str_to_option_arg.end()) {
                //Start of an argument
                auto& arg = iter->second;

                std::string value;
                if (arg.action() == "store_true") {
                    value = "true";
                    arg_values.set(arg.dest(), value);

                } else if (arg.action() == "store_false") {
                    value = "false";
                    arg_values.set(arg.dest(), value);

                } else {
                    assert(arg.action() == "store");


                    size_t max_values_to_read = 0;
                    size_t min_values_to_read = 0;
                    if (arg.nargs() == '1') {
                        max_values_to_read = 1;
                        min_values_to_read = 1;
                    } else if (arg.nargs() == '?') {
                        max_values_to_read = 1;
                        min_values_to_read = 0;
                    } else if (arg.nargs() == '*') {
                        max_values_to_read = std::numeric_limits<size_t>::max();
                        min_values_to_read = 0;
                    } else {
                        assert (arg.nargs() == '+');
                        max_values_to_read = std::numeric_limits<size_t>::max();
                        min_values_to_read = 1;
                    }

                    std::vector<std::string> values;
                    size_t nargs_read;
                    for (nargs_read = 0; nargs_read < max_values_to_read; ++nargs_read) {
                        std::string str = arg_strs[i + 1 + nargs_read];

                        if (is_argument(str)) break;

                        values.push_back(str);
                    }

                    if (nargs_read < min_values_to_read) {
                        std::stringstream msg;
                        msg << "Expected at least " << min_values_to_read << " values for argument '" << arg_strs[i] << "'";
                        throw ArgParseError(msg.str().c_str());
                    }
                    assert (nargs_read <= max_values_to_read);

                    arg_values.set(arg.dest(), values);

                    i += nargs_read; //Skip over the values
                }

            } else {
                if (positional_args.empty()) {
                    //Unrecognized
                    std::stringstream ss;
                    ss << "Unrecognized command-line argument '" << arg_strs[i] << "'";
                    throw ArgParseError(ss.str().c_str());
                } else {
                    //Positional argument
                    auto arg = positional_args.front();
                    positional_args.pop_front();

                    auto value = arg_strs[i];
                    
                    arg_values.set(arg.dest(), value);
                }
            }
        }

        return arg_values;
    }

    void ArgumentParser::print_help() {
        formatter_->set_parser(this);
        os_ << formatter_->format_usage();
        os_ << formatter_->format_description();
        os_ << formatter_->format_arguments();
        os_ << formatter_->format_epilog();
    }

    std::string ArgumentParser::prog() const { return prog_; }
    std::string ArgumentParser::description() const { return description_; }
    std::string ArgumentParser::epilog() const { return epilog_; }
    std::vector<ArgumentGroup> ArgumentParser::argument_groups() const { return argument_groups_; }

    /*
     * ArgumentGroup
     */
    ArgumentGroup::ArgumentGroup(std::string description_str)
        : description_(description_str)
        {}

    Argument& ArgumentGroup::add_argument(std::string option) {
        return add_argument(option, std::string());
    }

    Argument& ArgumentGroup::add_argument(std::string long_opt, std::string short_opt) {
        arguments_.push_back(Argument(long_opt, short_opt));
        return arguments_[arguments_.size() - 1];
    }

    std::string ArgumentGroup::description() const { return description_; }
    std::vector<Argument> ArgumentGroup::arguments() const { return arguments_; }

    /*
     * Argument
     */
    Argument::Argument(std::string long_opt, std::string short_opt)
        : long_opt_(long_opt)
        , short_opt_(short_opt) {

        if (long_opt_.size() < 1) {
            throw ArgParseError("Argument must be at least one character long");
        }

        auto dashes_name = split_leading_dashes(long_opt_);

        if (dashes_name[0].size() == 1 && !short_opt_.empty()) {
            throw ArgParseError("Long option must be specified before short option");
        } else if (dashes_name[0].size() > 2) {
            throw ArgParseError("More than two dashes in argument name");
        }

        //Set defaults
        dest_ = dashes_name[1];
        metavar_ = toupper(dest_);
    }

    Argument& Argument::help(std::string help_str) {
        help_ = help_str;
        return *this;
    }

    Argument& Argument::nargs(char nargs_type) {
        std::array<char,4> valid_nargs = {'1', '?', '*', '+'};

        auto iter = std::find(valid_nargs.begin(), valid_nargs.end(), nargs_type);
        if (iter == valid_nargs.end()) {
            std::stringstream ss;
            ss << "Invalid argument to nargs (must be one of: ";
            bool first = true;
            for (char c : valid_nargs) {
                if (!first) {
                    ss << ", ";
                }
                ss << "'" << c << "'";
            }
            ss << ")";
            throw ArgParseError(ss.str().c_str());
        }

        nargs_ = nargs_type;
        return *this;
    }

    Argument& Argument::metavar(std::string metavar_str) {
        metavar_ = metavar_str;
        return *this;
    }

    Argument& Argument::choices(std::vector<std::string> choice_values) {
        choices_ = choice_values;
        return *this;
    }

    Argument& Argument::dest(std::string dest_str) {
        dest_ = dest_str;
        return *this;
    }

    Argument& Argument::action(std::string action_str) {
        std::array<const char*,3> valid_actions = {"store_true", "store_false", "store"};

        auto iter = std::find(valid_actions.begin(), valid_actions.end(), action_str);
        if (iter == valid_actions.end()) {
            throw ArgParseError("Invalid argument to nargs (must be 'store', 'store_true', or 'store_false')");
        }
        
        action_ = action_str;
        return *this;
    }

    Argument& Argument::required(bool is_required) {
        required_ = is_required;
        return *this;
    }

    Argument& Argument::show_in_usage(bool show) {
        show_in_usage_ = show;
        return *this;
    }

    std::string Argument::long_option() const { return long_opt_; }
    std::string Argument::short_option() const { return short_opt_; }
    std::string Argument::help() const { return help_; }
    std::string Argument::default_value() const { return default_values_[0]; }
    char Argument::nargs() const { return nargs_; }
    std::string Argument::metavar() const { return metavar_; }
    std::vector<std::string> Argument::choices() const { return choices_; }
    std::string Argument::dest() const { return dest_; }
    std::string Argument::action() const { return action_; }
    bool Argument::required() const { return required_; }
    bool Argument::show_in_usage() const { return show_in_usage_; }

    bool Argument::positional() const {
        assert(long_option().size() > 1);
        return long_option()[0] != '-';
    }

    /*
     * ArgValues
     */
    bool ArgValues::has_argument(std::string name) const {
        return count(name) > 0;
    }

    size_t ArgValues::count(std::string name) const {
        return values_.count(name);
    }

    void ArgValues::add(std::string dest, std::string value) {
        values_.insert(std::make_pair(dest, value));
    }

    void ArgValues::set(std::string dest, std::string value) {
        auto range = values_.equal_range(dest);
        values_.erase(range.first, range.second);

        values_.insert(std::make_pair(dest, value));
    }

    void ArgValues::set(std::string dest, std::vector<std::string> values) {
        auto range = values_.equal_range(dest);
        values_.erase(range.first, range.second);


        for (auto& value : values) {
            values_.insert(std::make_pair(dest, value));
        }
    }

} //namespace
