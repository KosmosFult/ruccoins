//
// Created by flt on 5/14/24.
//

#include <rpc/client.h>
#include <unistd.h>
#include "../src/pbft/include/bft.h"
int main(){
    rpc::client cl("127.0.0.1", 19090);
    PBFT::Request r(1, 8942894, "I have a request!");
    cl.call("GetRequest", r);
    return 0;
}