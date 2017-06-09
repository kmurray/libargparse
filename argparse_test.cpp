#include "argparse.hpp"

int main(int argc, const char** argv) {

    struct Args {
        std::string architecture_file;
        std::string circuit;

        bool pack;
        bool place;
        bool route;

        bool timing_analysis;
        char slack_definition;
        bool echo_files;
        bool verify_file_digests;

        bool disp;
        bool auto_value;

        int long1;
        int long2;
        std::string c;
    };

    Args args;

    auto parser = argparse::ArgumentParser("Test parser for libargparse");
    parser.prog(argv[0]);
    parser.epilog("This is the epilog");

    auto& pos_grp = parser.add_argument_group("positional arguments:");

    pos_grp.add_argument(args.architecture_file, "architecture")
            .help("FPGA Architecture description file (XML)");

    pos_grp.add_argument(args.circuit, "circuit")
            .help("Circuit file (name if --blif_file specified)");

    auto& stage_grp = parser.add_argument_group("stage options:");

    stage_grp.add_argument(args.pack, "--pack")
            .help("Run packing")
            .action(argparse::Action::STORE_TRUE)
            .default_value(false);

    stage_grp.add_argument(args.place, "--place")
            .help("Run placement")
            .action(argparse::Action::STORE_TRUE)
            .default_value(false);

    stage_grp.add_argument(args.route, "--route")
            .help("Run routing")
            .action(argparse::Action::STORE_TRUE)
            .default_value(false);

    stage_grp.add_argument(args.route, "--analysis")
            .help("Run analysis")
            .action(argparse::Action::STORE_TRUE)
            .default_value(false);

    stage_grp.epilog("Analysis is always run after routing."
                     " If none of the stage options are specified, all stages are run.");

    auto& gen_grp = parser.add_argument_group("general options:");

    gen_grp.add_argument(args.timing_analysis, "--timing_analysis")
            .help("Controls whether timing analysis (and timing driven optimizations) are enabled.")
            .default_value(true)
            //.choices({true, false});
            //.choices<bool>({true, false});
            ;

    //gen_grp.add_argument(args.slack_definition, "--slack_definition")
            //.help("Sets the slack definition used by the classic timing analyyzer")
            //.default_value('R')
            //.choices({'R', 'I', 'S', 'G', 'C', 'N'});

    gen_grp.add_argument(args.echo_files, "--echo_file")
            .help("Generate echo files of key internal data structures. Useful for debugging VPR, and typically end in .echo")
            .default_value(false);
            //.choices({true, false});

    gen_grp.add_argument(args.verify_file_digests, "--verify_file_digests")
            .help("Verify that intermediate files loaded by VPR (e.g. previous packing/placement/routing) are consistent")
            .default_value(true);
            //.choices({true, false});

    auto& gfx_grp = parser.add_argument_group("graphics options:");
    gfx_grp.add_argument(args.disp, "--disp")
            .help("Enable or disable interactive graphics")
            .default_value(true)
            //.choices({true, false})
            ;

    //gfx_grp.add_argument(args.auto_value, "--auto")
            //.help("Controls how often VPR pauses for interactive"
                  //" graphics (and Proceed must be clicked to continue)."
                  //" Higher values reduce the frequency of pauses")
            //.default_value(1)
            //.choices({0, 1, 2});

    //auto& file_grp = parser.add_argument_group("filename options:");

    //auto& nlist_grp = parser.add_argument_group("netlist options:");

    //auto& pack_grp = parser.add_argument_group("packing options:");

    //auto& pack_timing_grp = parser.add_argument_group("timing-driven packing options:");

    //auto& place_grp = parser.add_argument_group("placement options:");

    //auto& place_timing_grp = parser.add_argument_group("timing-driven placement options:");

    //auto& route_grp = parser.add_argument_group("routing options:");

    //auto& route_timing_grp = parser.add_argument_group("timing-driven routing options:");

    parser.add_argument(args.long1, "--long")
              .help("Help for a long option")
              .default_value(9)
              .metavar("LONG_VALUE");

    parser.add_argument(args.long2, "--long2", "-l");

    parser.add_argument(args.c, "-c")
                //.choices({"a", "b", "c", "d"});
                ;

    parser.add_argument(args.c, "pos1")
                .help("A positional argument");

    parser.add_argument(args.disp, "-disp")
                .action(argparse::Action::STORE_TRUE)
                .help("Enable display");
    parser.add_argument(args.disp, "-nodisp")
                .action(argparse::Action::STORE_FALSE)
                .help("Disable display");

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
