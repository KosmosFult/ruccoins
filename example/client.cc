#include <iostream>
#include "rpc/client.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // Creating a client that connects to the localhost on port 8080
    rpc::client client("127.0.0.1", 8080);

    // Calling a function with paramters and converting the result to int
    auto result = client.call("add", 2, 3).as<int>();
    std::cout << "The results is: " << result << std::endl;

    std::ifstream f("/home/flt/workspace/bitcoin/example/test.json");
    json data = json::parse(f);
    std::cout << data["name"] << std::endl;
    return 0;
}