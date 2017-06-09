#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <iosfwd>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include <map>

#include "argparse_formatter.hpp"
#include "argparse_default_converter.hpp"
#include "argparse_error.hpp"

namespace argparse {

    class Argument;
    class ArgumentGroup;
    class ArgValues;

    enum class Action {
        STORE,
        STORE_TRUE,
        STORE_FALSE,
        APPEND,
        COUNT
    };

    class ArgumentParser {
        public:
            ArgumentParser(std::string description_str=std::string(), std::ostream& os=std::cout);
            ArgumentParser& prog(std::string prog);
            ArgumentParser& epilog(std::string prog);

            template<typename T, typename Converter=DefaultConverter<T>>
            Argument& add_argument(T& dest, std::string option);

            template<typename T, typename Converter=DefaultConverter<T>>
            Argument& add_argument(T& dest, std::string long_opt, std::string short_opt);

            ArgumentGroup& add_argument_group(std::string description_str);

            void parse_args(int argc, const char** argv);
            void parse_args(std::vector<std::string> args);

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

            template<typename T, typename Converter=DefaultConverter<T>>
            Argument& add_argument(T& dest, std::string option);

            template<typename T, typename Converter=DefaultConverter<T>>
            Argument& add_argument(T& dest, std::string long_opt, std::string short_opt);

            ArgumentGroup& epilog(std::string str);

        public:
            std::string description() const;
            std::string epilog() const;
            const std::vector<std::shared_ptr<Argument>>& arguments() const;
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
            std::string epilog_;
            std::vector<std::shared_ptr<Argument>> arguments_;
    };

    class Argument {
        public:
            Argument(std::string long_opt, std::string short_opt);
        public: //Mutators
            Argument& help(std::string help_str);

            Argument& default_value(const std::string& default_val);

            Argument& action(Action action);
            Argument& required(bool is_required);
            Argument& metavar(std::string metavar_sr);
            Argument& nargs(char nargs_type);

            Argument& choices(std::vector<std::string> choice_values);

            Argument& show_in_usage(bool show);

            virtual void set_dest_to_default() = 0;

        public: //Accessors
            std::string long_option() const;
            std::string short_option() const;

            std::string help() const;
            //virtual std::string default_value() const = 0;
            char nargs() const;
            std::string metavar() const;
            std::vector<std::string> choices() const;
            Action action() const;
            bool required() const;

            std::string default_value() const;

            bool show_in_usage() const;
            bool positional() const;
        public: //Lifetime
            virtual ~Argument() {}
            Argument(const Argument&) = default;
            Argument(Argument&&) = default;
            Argument& operator=(const Argument&) = delete;
            Argument& operator=(const Argument&&) = delete;
        private: //Data
            std::string long_opt_;
            std::string short_opt_;

            std::string help_;
            std::string metavar_;
            char nargs_ = '1';
            std::vector<std::string> choices_;
            Action action_ = Action::STORE;
            bool required_ = false;

            std::string default_value_;

            bool show_in_usage_ = true;
    };


    template<typename T, typename Converter=DefaultConverter<T>>
    class SingleValueArgument : public Argument {
        public: //Constructors
            SingleValueArgument(T& dest, std::string long_opt, std::string short_opt);
        public: //Mutators
            void set_dest_to_default() override;

            void set_dest_to_value_from_str(std::string value);
            void set_dest_to_value(T value);
        private: //Data
            T& dest_;
    };

#ifdef MULTI_VALUE
    template<typename T>
    class MultiValueArgument : public Argument {
        public: //Constructors
            MultiValueArgument(std::vector<T>& dest, std::string long_opt, std::string short_opt);
        public: //Mutators
            MultiValueArgument& default_value(std::vector<T> default_val);

            void set_dest_to_value(std::vector<T> value);

            void set_dest_to_default() override;
        public: //Accessors
            std::string default_value() const override;

        private: //Data
            std::vector<T>& dest_;
            std::vector<T> default_values_;
    };
#endif

} //namespace

#include "argparse.tpp"

#endif
