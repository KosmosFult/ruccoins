#include <iostream>
#include "rpc/client.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <leveldb/db.h>
#include <string>
#include <openssl/ecdsa.h>
using json = nlohmann::json;

using namespace leveldb;

const std::string dbname = "/home/flt/workspace/bitcoin/testdb";
int main() {

    // RPC 的例子
    // Creating a client that connects to the localhost on port 8080
    rpc::client client("127.0.0.1", 8080);

    // Calling a function with paramters and converting the result to int
    auto result = client.call("add", 2, 3).as<int>();
    std::cout << "The resultsss is: " << result << std::endl;


    // Json的例子
    std::ifstream f("/home/flt/workspace/bitcoin/example/test.json");
    json data = json::parse(f);
    std::cout << data["name"] << std::endl;

    // Leveldb的例子
    Options options;
    ReadOptions roptions;
    options.create_if_missing = true;
    WriteOptions woptions;
    DB* mydb;
    auto status = DB::Open(options, dbname, &mydb);

    assert(status.ok());
    Slice mkey("Alice");
    Slice mvalue("1234567876");
    std::cout << dbname << std::endl;
    mydb->Put(woptions, mkey, mvalue);
    std::string getval;
    mydb->Get(roptions, "Tom", &getval);    
    std::cout << getval << std::endl;
    mydb->Get(roptions, "Alice", &getval);    
    std::cout << getval << std::endl;

    delete mydb;
    return 0;
}