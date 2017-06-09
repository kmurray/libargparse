#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <iosfwd>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <map>

#include "argparse_formatter.hpp"

namespace argparse {

    class Argument;
    class ArgumentGroup;
    class ArgValues;

    class ArgumentParser {
        public:
            ArgumentParser(std::string description_str=std::string(), std::ostream& os=std::cout);
            ArgumentParser& prog(std::string prog);
            ArgumentParser& epilog(std::string prog);

            Argument& add_argument(std::string option);
            Argument& add_argument(std::string long_opt, std::string short_opt);

            ArgumentGroup& add_argument_group(std::string description_str);

            ArgValues parse_args(int argc, const char** argv);
            ArgValues parse_args(std::vector<std::string> args);

            void print_help();
        public:
            std::string prog() const;
            std::string description() const;
            std::string epilog() const;
            std::vector<ArgumentGroup> argument_groups() const;

        private:
            std::string prog_;
            std::string description_;
            std::string epilog_;
            std::vector<ArgumentGroup> argument_groups_;

            Formatter* formatter_;
            std::ostream& os_;
    };

    class ArgumentGroup {
        public:
            Argument& add_argument(std::string option);

            Argument& add_argument(std::string long_opt, std::string short_opt);

        public:
            std::string description() const;
            std::vector<Argument> arguments() const;
        public:
            ArgumentGroup(const ArgumentGroup&) = default;
            ArgumentGroup(ArgumentGroup&&) = default;
            ArgumentGroup& operator=(const ArgumentGroup&) = delete;
            ArgumentGroup& operator=(const ArgumentGroup&&) = delete;
        private:
            friend class ArgumentParser;
            ArgumentGroup(std::string description_str=std::string());
        private:
            std::string description_;
            std::vector<Argument> arguments_;
    };

    class Argument {
        public: //Mutators
            Argument& help(std::string help_str);

            template<typename T>
            Argument& default_value(T default_val);

            template<typename T>
            Argument& default_value(std::vector<T> default_vals);

            Argument& nargs(char nargs_type);
            Argument& metavar(std::string metavar_sr);
            Argument& choices(std::vector<std::string> choice_values);
            Argument& dest(std::string dest_str);
            Argument& action(std::string action_str);
            Argument& required(bool is_required);

            Argument& show_in_usage(bool show);

        public: //Accessors
            std::string long_option() const;
            std::string short_option() const;

            std::string help() const;
            std::string default_value() const;
            char nargs() const;
            std::string metavar() const;
            std::vector<std::string> choices() const;
            std::string dest() const;
            std::string action() const;
            bool required() const;

            bool show_in_usage() const;
            bool positional() const;
        public: //Lifetime
            Argument(const Argument&) = default;
            Argument(Argument&&) = default;
            Argument& operator=(const Argument&) = delete;
            Argument& operator=(const Argument&&) = delete;

        private:
            friend class ArgumentParser;
            friend class ArgumentGroup;
            Argument(std::string long_opt, std::string short_opt=std::string());

        private:
            std::string long_opt_;
            std::string short_opt_;

            std::string help_;
            std::vector<std::string> default_values_;
            std::string metavar_;
            char nargs_ = '?';
            std::vector<std::string> choices_;
            std::string dest_;
            std::string action_ = "store";
            bool required_ = false;

            bool show_in_usage_ = true;
    };

    class ArgValues {
        public:
            //Returns true if the argument 'name' has a specified value
            bool has_argument(std::string name) const;

            //Returns the number of values associated with 'name'
            size_t count(std::string name) const;

            //Returns the single value associated with 'name' 
            // Throws an exception if there is not precisely one value.
            template<typename T>
            T value_as(std::string key) const;

            //Returns the one, or more value associated with 'name' 
            // Throws an exception if there are no associated values.
            template<typename T>
            std::vector<T> values_as(std::string key) const;

        public:
            void set(std::string arg_name, std::string value);
            void set(std::string arg_name, std::vector<std::string> values);
            void add(std::string arg_name, std::string value);
        private:
            std::multimap<std::string,std::string> values_;
    };

    class ArgParseError : public std::runtime_error {
        using std::runtime_error::runtime_error; //Constructors
    };

} //namespace

#include "argparse.tpp"

#endif
