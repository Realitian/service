#include "service.h"

extern std::string inputPath;
extern std::string outputPath;
extern std::string command;

int main(int argc, char *argv[])
{
    if ( argc > 2 )
    {
        inputPath = argv[1];
        outputPath = argv[2];
    }

    if ( argc > 3 )
    {
        command = argv[3];
    }

    boost::asio::io_service io_service;
    server s(io_service, 10000);
    io_service.run();

    return 0;
}
