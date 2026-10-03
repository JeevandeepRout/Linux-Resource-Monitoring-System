#include <iostream>
#include "Server.h"

int main() {

    try {

        Server server(5000);

        server.start();

    }
    catch (const std::exception& e) {

        std::cerr << "Server error: "
                  << e.what()
                  << '\n';

        return 1;
    }

    return 0;
}
