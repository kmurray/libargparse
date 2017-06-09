#include "argparse.hpp"

int main(int argc, const char** argv) {

    struct Args {
        std::string architecture_file;
        std::string circuit;

        bool disp;
        bool auto_value;

        bool pack;
        bool place;
        bool route;

        bool timing_analysis;
        const char* slack_definition;
        bool echo_files;
        bool verify_file_digests;

        std::string blif_file;
        std::string net_file;
        std::string place_file;
        std::string route_file;
        std::string sdc_file;
        std::string outfile_prefix;

        bool absorb_buffer_luts;
        bool sweep_dangling_primary_ios;
        bool sweep_dangling_nets;
        bool sweep_dangling_blocks;
        bool sweep_constant_primary_outputs;

        bool connection_driven_clustering;
        bool allow_unrelated_clustering;
        float alpha_clustering;
        float beta_clustering;
        bool timing_driven_clustering;
        std::string cluster_seed_type;

        bool full_stats;
        bool gen_post_synthesis_netlist;
    };

    struct OnOff {
        bool from_str(std::string str) {
            if      (str == "on")  return true;
            else if (str == "off") return false;
            throw argparse::ArgParseConversionError("Invalid conversion");
        }

        std::string to_str(bool val) {
            if (val) return "on";
            return "off";
        }

        std::vector<std::string> default_choices() {
            return {"on", "off"};
        }
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

    auto& gfx_grp = parser.add_argument_group("graphics options:");
    gfx_grp.add_argument<bool,OnOff>(args.disp, "--disp")
            .help("Enable or disable interactive graphics")
            .default_value("off");
    gfx_grp.add_argument(args.auto_value, "--auto")
            .help("Controls how often VPR pauses for interactive"
                  " graphics (and Proceed must be clicked to continue)."
                  " Higher values reduce the frequency of pauses")
            .default_value("1")
            .choices({"0", "1", "2"});

    auto& stage_grp = parser.add_argument_group("stage options:");

    stage_grp.add_argument<bool,OnOff>(args.pack, "--pack")
            .help("Run packing")
            .action(argparse::Action::STORE_TRUE)
            .default_value("off");
    stage_grp.add_argument<bool,OnOff>(args.place, "--place")
            .help("Run placement")
            .action(argparse::Action::STORE_TRUE)
            .default_value("off");
    stage_grp.add_argument<bool,OnOff>(args.route, "--route")
            .help("Run routing")
            .action(argparse::Action::STORE_TRUE)
            .default_value("off");
    stage_grp.add_argument<bool,OnOff>(args.route, "--analysis")
            .help("Run analysis")
            .action(argparse::Action::STORE_TRUE)
            .default_value("off");

    stage_grp.epilog("If none of the stage options are specified, all stages are run."
                     " Analysis is always run after routing.");

    auto& gen_grp = parser.add_argument_group("general options:");

    gen_grp.add_argument<bool,OnOff>(args.timing_analysis, "--timing_analysis")
            .help("Controls whether timing analysis (and timing driven optimizations) are enabled.")
            .default_value("on") ;
    gen_grp.add_argument(args.slack_definition, "--slack_definition")
            .help("Sets the slack definition used by the classic timing analyyzer")
            .default_value("R")
            .choices({"R", "I", "S", "G", "C", "N"});
    gen_grp.add_argument<bool,OnOff>(args.echo_files, "--echo_file")
            .help("Generate echo files of key internal data structures. Useful for debugging VPR, and typically end in .echo")
            .default_value("off");
    gen_grp.add_argument<bool,OnOff>(args.verify_file_digests, "--verify_file_digests")
            .help("Verify that intermediate files loaded by VPR (e.g. previous packing/placement/routing) are consistent")
            .default_value("on");

    auto& file_grp = parser.add_argument_group("filename options:");

    file_grp.add_argument(args.blif_file, "--blif_file")
            .help("Path to technology mapped circuit in BLIF format");
    file_grp.add_argument(args.net_file, "--net_file")
            .help("Path to packed netlist file");
    file_grp.add_argument(args.place_file, "--place_file")
            .help("Path to placement file");
    file_grp.add_argument(args.route_file, "--route_file")
            .help("Path to routing file");
    file_grp.add_argument(args.sdc_file, "--sdc_file")
            .help("Path to iming constraints file in SDC format");
    file_grp.add_argument(args.outfile_prefix, "--outfile_prefix")
            .help("Prefix for output files");


    auto& netlist_grp = parser.add_argument_group("netlist options:");
    netlist_grp.add_argument<bool,OnOff>(args.absorb_buffer_luts, "--absorb_buffer_luts")
            .help("Controls whether LUTS programmed as buffers are absorbed by downstream logic")
            .default_value("on");
    netlist_grp.add_argument<bool,OnOff>(args.sweep_dangling_primary_ios, "--sweep_dangling_primary_ios")
            .help("Controls whether dangling primary inputs and outputs are removed from the netlist")
            .default_value("on");
    netlist_grp.add_argument<bool,OnOff>(args.sweep_dangling_nets, "--sweep_dangling_nets")
            .help("Controls whether dangling nets are removed from the netlist")
            .default_value("on");
    netlist_grp.add_argument<bool,OnOff>(args.sweep_dangling_blocks, "--sweep_dangling_blocks")
            .help("Controls whether dangling blocks are removed from the netlist")
            .default_value("on");
    netlist_grp.add_argument<bool,OnOff>(args.sweep_constant_primary_outputs, "--sweep_constant_primary_outputs")
            .help("Controls whether primary outputs driven by constant values are removed from the netlist")
            .default_value("off");

    auto& pack_grp = parser.add_argument_group("packing options:");
    pack_grp.add_argument<bool,OnOff>(args.connection_driven_clustering, "--connection_driven_clustering")
            .help("Controls whether or not packing prioritizes the absorption of nets with fewer connections into a complex logic block over nets with more connections")
            .default_value("on");
    pack_grp.add_argument<bool,OnOff>(args.allow_unrelated_clustering, "--allow_unrelated_clustering")
            .help("Controls whether or not primitives with no attraction to the current cluster can be packed into it")
            .default_value("on");
    pack_grp.add_argument(args.alpha_clustering, "--alpha_clustering")
            .help("Parameter that weights the optimization of timing vs area. 0.0 focuses solely on area, 1.0 solely on timing.")
            .default_value("0.75");
    pack_grp.add_argument(args.beta_clustering, "--beta_clustering")
            .help("Parameter that weights the absorption of small nets vs signal sharing. 0.0 focuses solely on sharing, 1.0 solely on small net absoprtion. Only meaningful if --connection_driven_clustering=on")
            .default_value("0.9");
    pack_grp.add_argument<bool,OnOff>(args.timing_driven_clustering, "--timing_driven_clustering")
            .help("Controls whether custering optimizes for timing")
            .default_value("on");
    pack_grp.add_argument(args.cluster_seed_type, "--cluster_seed_type")
            .help("Controls how primitives are chosen as seeds.")
            .default_value("blend")
            .choices({"blend", "timing", "max_inputs"});

    //auto& pack_timing_grp = parser.add_argument_group("timing-driven packing options:");

    //auto& place_grp = parser.add_argument_group("placement options:");

    //auto& place_timing_grp = parser.add_argument_group("timing-driven placement options:");

    //auto& route_grp = parser.add_argument_group("routing options:");

    //auto& route_timing_grp = parser.add_argument_group("timing-driven routing options:");

    auto& analysis_grp = parser.add_argument_group("analysis options:");

    analysis_grp.add_argument<bool,OnOff>(args.full_stats, "--full_stats")
                .help("Print extra statistics about the circuit and it's routing (useful for wireability analysis)")
                .default_value("off");

    analysis_grp.add_argument<bool,OnOff>(args.gen_post_synthesis_netlist, "--gen_post_synthesis_netlist")
                .help("Generates the post-synthesis netlist (in BLIF and Verilog) along with delay information (in SDF)")
                .default_value("off");

#if 0
    parser.add_argument(args.long1, "--long")
              .help("Help for a long option")
              .default_value(9)
              .metavar("LONG_VALUE");

    parser.add_argument(args.long2, "--long2", "-l");

    //parser.add_argument(args.c, "-c")
                //.choices({"a", "b", "c", "d"});
                //;

    parser.add_argument(args.c, "pos1")
                .help("A positional argument");

    parser.add_argument(args.disp, "-disp")
                .action(argparse::Action::STORE_TRUE)
                .help("Enable display");
    parser.add_argument(args.disp, "-nodisp")
                .action(argparse::Action::STORE_FALSE)
                .help("Disable display");
#endif
    parser.print_help();

    parser.parse_args(argc, argv);

    std::cout << "args.disp: " << args.disp << "\n";

    //for(auto& arg : args) {
        //std::cout << arg.name() << ": " << arg.value() << "\n";
    //}

    return 0;
}
