#include "argparse.hpp"

int main(int argc, const char** argv) {

    auto parser = argparse::ArgumentParser("Test parser for libargparse")
                                .prog(argv[0])
                                .epilog("This is the epilog");

    parser.add_argument("--long")
              .help("Help for a long option")
              .default_value(9)
              .metavar("LONG_VALUE");

    parser.add_argument("--long2", "-l");

    parser.add_argument("-c")
                .choices({"a", "b", "c", "d"});

    parser.print_help();

    auto args = parser.parse_args(argc, argv);

    //for(auto& arg : args) {
        //std::cout << arg.name() << ": " << arg.value() << "\n";
    //}

    return 0;
}
