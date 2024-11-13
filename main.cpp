#include "main.hpp"

int main( int ac, char **av ) {
	if (ac != 3) {
		std::cerr << "Usage:  ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	Server server(atoi(av[1]), av[2]);
	server.start();

	std::cout << "Waiting for connection..." << std::endl;

	server.connectUser();

	std::cout << "Connected client" << std::endl;

	std::string input;
	while (true) {
		std::getline(std::cin, input);
		if (input == "exit")
			break;
		input = "server " + input + "\n";
		send(server.getUser().getUserFd(), input.c_str(), input.length(), 0); // Must use poll
	}

	close(server.getUser().getUserFd());
	close(server.getSocket());
	return 0;
}