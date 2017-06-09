#include "argparse_formatter.hpp"

#include "argparse.hpp"

namespace argparse {

    /*
     * DefaultFormatter
     */
    void DefaultFormatter::set_parser(ArgumentParser* parser) {
        parser_ = parser;
    }

    std::string DefaultFormatter::format_usage() const {
        if (!parser_) throw ArgParseError("parser not initialized in help formatter");

        std::stringstream ss;
        ss << "usage: " << parser_->prog() << " [options]" << "\n";
        return ss.str();
    }

    std::string DefaultFormatter::format_description() const {
        if (!parser_) throw ArgParseError("parser not initialized in help formatter");

        std::stringstream ss;
        ss << "\n";
        ss << parser_->description() << "\n";
        return ss.str();
    }

    std::string DefaultFormatter::format_arguments() const {
        if (!parser_) throw ArgParseError("parser not initialized in help formatter");

        std::stringstream ss;

        for (const auto& group : parser_->argument_groups()) {
            auto args = group.arguments();
            if (args.size() > 0) {
                ss << "\n";
                ss << group.description() << "\n";
                for (const auto& arg : args) {

                    //name/option
                    ss << "  ";
                    auto short_opt = arg.short_option();
                    auto long_opt = arg.long_option();
                    if (!short_opt.empty()) {
                        ss << short_opt;
                    }
                    if (!long_opt.empty()) {
                        if (!short_opt.empty()) {
                            ss << ", ";
                        }
                        ss << long_opt;
                    }

                    //Choices
                    auto choices = arg.choices();
                    if (!choices.empty()) {
                        ss << " {";
                        bool first = true;
                        for(auto choice : choices) {
                            if (!first) {
                                ss << ", ";
                            }
                            ss << choice;
                            first = false;
                        }
                        ss << "}";
                    }

                    ss << "\t";

                    //Argument help
                    ss << arg.help();

                    //Default
                    if (!arg.default_value().empty()) {
                        ss << " (Default: " << arg.default_value() << ")" << "\n";
                    }
                    ss << "\n";
                }
            }
        }

        return ss.str();
    }

    std::string DefaultFormatter::format_epilog() const {
        if (!parser_) throw ArgParseError("parser not initialized in help formatter");

        std::stringstream ss;
        ss << "\n";
        ss << parser_->epilog() << "\n";
        return ss.str();
    }

} //namespace
