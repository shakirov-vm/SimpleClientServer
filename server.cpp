
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <string>
#include <unistd.h>

//namespace client_server {

//compile: g++ server.cpp -o server -lpthread

using tcp = boost::asio::ip::tcp;

const int max_length = 1024;

void session(tcp::socket sock) {

	try {

		for(;;) {

			int generated = std::rand() % 10000;
			if (generated < 1000) generated += 1000;
			std::string num = std::to_string(generated);
			std::cout << "generated num is " << num << std::endl;

			for (;;) {

				std::string reply(4, '\0');

				boost::system::error_code error;
				std::cout << "Before first read" << std::endl;
	            size_t length = boost::asio::read(sock, boost::asio::buffer(reply, reply.size()), error); // Is it blocked there?

				std::cout << "After first read" << std::endl;
				if (length != 4) std::cout << "Oops, len is " << length << std::endl;
				if (error == boost::asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw boost::system::system_error(error); // Some other error.
				
				int beefs = 0, cows = 0;		 // 4 because of 4-digit number
				for (int i = 0; i < 4; i++) {
					
					if (num[i] == reply[i]) {
						beefs++;
						cows--;
					}
					for (int j = 0; j < 4; j++) {
						if (num[i] == reply[j]) cows++;
					}
				}

				std::string request;

				if (beefs == 4) { 
					request = std::string("You win\n");
				} else {
					char data_1[21] = {0};
					std::sprintf(data_1, "beefs - %d, cows - %d\n", beefs, cows);
					request = std::string(data_1);
					// request = std::string("beefs - %d, cows - %d\n", beefs, cows);
				}
				length = request.size();
				std::cout << "server answer " << request << "and length is " << length << std::endl;
				boost::asio::write(sock, boost::asio::buffer(&length, sizeof(size_t)));
				boost::asio::write(sock, boost::asio::buffer(request, length));

				if (length == std::string("You win\n").size()) {
					
					std::cout << "Yes, there" << std::endl;

           			char answer = '\0';
					boost::asio::read(sock, boost::asio::buffer(&answer, sizeof(char)));
					std::cout << "READ ANSWER" << std::endl;
					if (answer == 'n') return;
					break;
				}
			}
		}

	} catch (std::exception& e) {
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

void server(boost::asio::io_context& io_context, unsigned short port) {

	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
	for (;;) {

		std::thread(session, acceptor.accept()).detach();
	}
}

int main(int argc, char* argv[]) {

	try {

		if (argc != 2) {
			std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
			return 1;
		}

		boost::asio::io_context io_context;
		server(io_context, std::atoi(argv[1]));

	} catch (std::exception& err) {
		std::cerr << "Exception: " << err.what() << "\n";
	}

	return 0;
}

//}