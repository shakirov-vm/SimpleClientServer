
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

//compile: g++ client.cpp -o client -lpthread

//namespace client_server {

using boost::asio::ip::tcp;

void session(tcp::socket& sock) {

    for(;;) {

        size_t request_num;
        std::cout << "Enter message: ";
        std::cin  >> request_num; // Need check this number!

        std::string request(4, '\0'); // because we expect 4-digit number
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max());
        //std::getline(std::cin, request); // need minimum 4
        //std::getline(std::cin, request); // need minimum 4
        
        request = std::to_string(request_num);
        std::cout << "GETLINE WAS DOING <" << request << ">" << std::endl;
        boost::asio::write(sock, boost::asio::buffer(request));

        size_t reply_length;
        boost::asio::read(sock, boost::asio::buffer(&reply_length, sizeof(size_t)));

        std::string reply(reply_length, '0');
        boost::asio::read(sock, boost::asio::buffer(reply));

        std::cout << "Reply is: " << reply << std::endl;

        bool ret = (reply == "You win\n");

        if (ret) { // Is it OK??
            std::cout << "Do you want play one more time? y/n" << std::endl;

            char answer = '\0';
            while (answer != 'y' && answer != 'n') {
                std::cin >> answer;
            //  std::cin.ignore
            } 
            std::cout << "answer " << answer << std::endl;
            boost::asio::write(sock, boost::asio::buffer(&answer, sizeof(char)));

            std::cout << "WRITE ANSWER" << std::endl;
            if (answer == 'n') return;
        }
    }
}

int main(int argc, char* argv[]) {

    try {
    
        if (argc != 3) {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::socket sock(io_context);
        tcp::resolver resolver(io_context); // make endpoint
        boost::asio::connect(sock, resolver.resolve(argv[1], argv[2]));

        session(sock);

        std::cout << "Game over" << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

//}