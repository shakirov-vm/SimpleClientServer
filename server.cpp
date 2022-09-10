
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <string>

//namespace client_server {

//using WIN_STRING = "You win\n";
//#define WIN_STRING "You win\n"

//compile: g++ server.cpp -o server -lpthread

using tcp = boost::asio::ip::tcp;

const int max_length = 1024;

void session(tcp::socket sock) {

	try {

		const char num[4] = {'2', '5', '7', '8'};

		for (;;) {

			std::string reply("0000");
			int beefs = 0, cows = 0;

			//boost::system::error_code error;
			
            size_t length = boost::asio::read(sock, boost::asio::buffer(reply, reply.size()));
			std::cout << "client enter " << reply << " and length " << length << std::endl;

			if (length != 4) std::cout << "Oops, len is " << length << std::endl;
			/*if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.*/

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
				length = request.size();
			}
			else {
				char data_1[21] = {0};
				std::sprintf(data_1, "beefs - %d, cows - %d\n", beefs, cows);
				request = std::string(data_1);
				// request = std::string("beefs - %d, cows - %d\n", beefs, cows);
			}
			length = request.size();
			std::cout << "server answer " << request << "and length is " << length << std::endl;
			boost::asio::write(sock, boost::asio::buffer(&length, sizeof(size_t)));
			boost::asio::write(sock, boost::asio::buffer(request, length));
			if (length == 8) return;
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