#include <algorithm>
#include <array>

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

    Args ArgumentParser::parse_args(int argc, const char** argv) {
        //TODO: implement
        return Args();
    }
    
    Args ArgumentParser::parse_args(std::vector<std::string> args) {
        //TODO: implement
        return Args();
    }

    ArgumentParser& ArgumentParser::prog(std::string prog_name) {
        prog_ = prog_name;
        return *this;
    }

    ArgumentParser& ArgumentParser::epilog(std::string epilog_str) {
        epilog_ = epilog_str;
        return *this;
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
            throw ArgParseError("Invalid argument to nargs (must be '1', '?', '*' or '+')");
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

    Argument& Argument::show_in_usage(bool show) {
        show_in_usage_ = show;
        return *this;
    }

    std::string Argument::long_option() const { return long_opt_; }
    std::string Argument::short_option() const { return short_opt_; }
    std::string Argument::help() const { return help_; }
    std::string Argument::default_value() const { return default_value_; }
    char Argument::nargs() const { return nargs_; }
    std::string Argument::metavar() const { return metavar_; }
    std::vector<std::string> Argument::choices() const { return choices_; }
    bool Argument::show_in_usage() const { return show_in_usage_; }

} //namespace
