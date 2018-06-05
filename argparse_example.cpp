#include "argparse.hpp"

using argparse::ArgValue;

struct Args {
    ArgValue<bool> do_foo;
    ArgValue<bool> enable_bar;
    ArgValue<std::string> filename;
    ArgValue<size_t> verbosity;
    ArgValue<bool> show_version;
    ArgValue<float> utilization;
};

struct OnOff {
    bool from_str(std::string str) {
        if      (str == "on")  return true;
        else if (str == "off") return false;
        std::stringstream msg;
        throw argparse::ArgParseConversionError("Invalid argument value");
    }

    std::string to_str(bool val) {
        if (val) return "on";
        return "off";
    }

    std::vector<std::string> default_choices() {
        return {"on", "off"};
    }
};

struct ZeroOneRange {
    float from_str(std::string str) {
        float value;
        std::stringstream ss(str);
        ss >> value;

        if (value < 0. || value > 1.) {
            std::stringstream msg;
            msg << "Value '" << value << "' out of expected range [0.0, 1.0]";
            throw argparse::ArgParseConversionError(msg.str());
        }
        return value;
    }

    std::string to_str(float value) {
        return std::to_string(value);
    }

    std::vector<std::string> default_choices() {
        return {};
    }
};

int main(int argc, const char** argv) {
    Args args;
    auto parser = argparse::ArgumentParser(argv[0], "My application description");

    parser.version("Version: 0.0.1");

    parser.add_argument(args.filename, "filename")
        .help("File to process");

    parser.add_argument(args.do_foo, "--foo")
        .help("Causes foo")
        .default_value("false")
        .action(argparse::Action::STORE_TRUE);

    parser.add_argument<bool,OnOff>(args.enable_bar, "--bar")
        .help("Control whether bar is enabled")
        .default_value("off");

    parser.add_argument(args.verbosity, "--verbosity", "-v")
        .help("Sets the verbosity")
        .default_value("1")
        .choices({"0", "1", "2"});
    parser.add_argument(args.show_version, "--version", "-V")
        .help("Show version information")
        .action(argparse::Action::VERSION);

    parser.add_argument<float,ZeroOneRange>(args.utilization, "--util")
        .help("Sets target utilization")
        .default_value("1.0");

    parser.add_argument<float,ZeroOneRange>(args.utilization, "--zulu")
        .help("Option with nargs='+'")
        .default_value("1.0");

    parser.parse_args(argc, argv);

    //Show the arguments
    std::cout << "args.filename: " << args.filename << "\n";
    std::cout << "args.do_foo: " << args.do_foo << "\n";
    std::cout << "args.verbosity: " << args.verbosity << "\n";
    std::cout << "args.utilization: " << args.utilization << "\n";
    std::cout << "\n";

    //Do work
    if (args.do_foo) {
        if (args.verbosity > 0) {
            std::cout << "Doing foo with " << args.filename << "\n";
        }
        if (args.verbosity > 1) {
            std::cout << "Doing foo step 1" << "\n";
            std::cout << "Doing foo step 2" << "\n";
            std::cout << "Doing foo step 3" << "\n";
        }
    }

    if (args.enable_bar) {
        if (args.verbosity > 0) {
            std::cout << "Bar is enabled" << "\n";
        }
    } else {
        if (args.verbosity > 0) {
            std::cout << "Bar is disabled" << "\n";
        }
    }

    return 0;
}

