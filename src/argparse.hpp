#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <iosfwd>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>

#include "argparse_formatter.hpp"

namespace argparse {

    class Argument;

    class ArgumentGroup;

    class Args;
    class ArgumentParser {
        public:
            ArgumentParser(std::string description_str=std::string(), std::ostream& os=std::cout);

            Argument& add_argument(std::string option);
            Argument& add_argument(std::string long_opt, std::string short_opt);

            ArgumentGroup& add_argument_group(std::string description_str);

            Args parse_args(int argc, const char** argv);
            Args parse_args(std::vector<std::string> args);

            ArgumentParser& prog(std::string prog);
            ArgumentParser& epilog(std::string prog);

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

    class Argument{
        public: //Mutators
            Argument& help(std::string help_str);

            template<class T>
            Argument& default_value(T default_val);

            Argument& nargs(char nargs_type);
            Argument& metavar(std::string metavar_sr);
            Argument& choices(std::vector<std::string> choice_values);
            Argument& dest(std::string dest_str);
            Argument& action(std::string action_str);
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

            bool show_in_usage() const;
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
            std::string default_value_;
            std::string metavar_;
            char nargs_ = '?';
            std::vector<std::string> choices_;
            std::string dest_;
            std::string action_ = "store";
            bool show_in_usage_ = true;
    };

    class Args {
        //TODO: implement

    };

    class ArgParseError : public std::runtime_error {
        using std::runtime_error::runtime_error; //Constructors
    };

} //namespace

#include "argparse.tpp"

#endif
