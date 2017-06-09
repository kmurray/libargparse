#include "argparse.hpp"

int main(int argc, const char** argv) {

    struct Args {
        int long1;
        int long2;
        std::string c;
    };

    Args args;

    auto parser = argparse::ArgumentParser("Test parser for libargparse")
                                .prog(argv[0])
                                .epilog("This is the epilog");

    parser.add_argument(args.long1, "--long")
              .help("Help for a long option")
              .default_value(9)
              .metavar("LONG_VALUE");

    parser.add_argument(args.long2, "--long2", "-l");

    parser.add_argument(args.c, "-c")
                .choices({"a", "b", "c", "d"});

    parser.print_help();

    parser.parse_args(argc, argv);

    std::cout << "args.long1: " << args.long1 << "\n";
    std::cout << "args.long2: " << args.long2 << "\n";
    std::cout << "args.c: " << args.c << "\n";

    //for(auto& arg : args) {
        //std::cout << arg.name() << ": " << arg.value() << "\n";
    //}

    return 0;
}
