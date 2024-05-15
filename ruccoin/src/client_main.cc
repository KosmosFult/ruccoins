//
// Created by flt on 3/18/24.
//

#include <iostream>
#include "client.h"
#include "cxxopts.h"

int main(int argc, char* argv[]){
    cxxopts::Options commandline_options("client node", "For testing");
    std::string config_json;
    commandline_options.add_options()
            ("f,configfile", "configure file of PBFT", cxxopts::value<std::string>(config_json));
    commandline_options.parse(argc, argv);
    ruccoin::client client(config_json);
    client.Run();
    return 0;
}