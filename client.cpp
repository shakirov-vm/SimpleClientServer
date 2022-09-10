
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

#include "header.hpp"

//compile: g++ client.cpp -o client -lpthread

//namespace client_server {

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{   
    try {
    
        if (argc != 3) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::socket sock(io_context);
        tcp::resolver resolver(io_context); // make endpoint
        boost::asio::connect(sock, resolver.resolve(argv[1], argv[2]));

        for(;;) {

            std::cout << "Enter message: ";

            std::string request("0000");
            //std::cin.getline(request, request.size()); // need minimum 4
            std::getline(std::cin, request);

            std::cout << "request_size - " << request.size() << std::endl;
            boost::asio::write(sock, boost::asio::buffer(request));

            size_t reply_length;
            boost::asio::read(sock, boost::asio::buffer(&reply_length, sizeof(size_t)));

            std::string reply(reply_length, '0');
            boost::asio::read(sock, boost::asio::buffer(reply, reply_length));

            std::cout << "Reply is: " << reply << std::endl;

            bool ret = (reply == "You win\n");

            if (!ret) { // Is it OK??
                std::cout << "Do you want play one more time? y/n" << std::endl;
            }
        }

        std::cout << "Game over" << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

//}