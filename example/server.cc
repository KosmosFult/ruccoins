#include <iostream>
#include "rpc/server.h"
#include <unistd.h>
#include "structure.h"
void foo() {
    std::cout << "foo was called!" << std::endl;
}

int my_sleep(){
    sleep(10);
    return 6;
}

int get_message(Message m){
    if(m.header.mtype == Mtype::prepare){
        std::cout << m.header.key << std::endl;
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    // Creating a server that listens on port 8080
    rpc::server srv(8080);

    // Binding the name "foo" to free function foo.
    // note: the signature is automatically captured
    srv.bind("foo", &foo);

    srv.bind("get_message", &get_message);

    // Binding a lambda function to the name "add".
    srv.bind("add", [](int a, int b) {
        return a + b;
    });

    srv.bind("my_sleep", &my_sleep);

    // Run the server loop.
    srv.run();

    return 0;
}