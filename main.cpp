#include "main.hpp"

int main( int ac, char **av ) {
	if (ac != 3) {
		std::cerr << "Usage:  ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	Server server(atoi(av[1]), av[2]);
	server.start();
	
	std::cout << "Waiting for connection..." << std::endl;

	int new_socket;
	int addrlen = sizeof(server.getAddress());

	// Accept a connection
	new_socket = accept(server.getSocket(), (struct sockaddr *)&server.getAddress(), (socklen_t*)&addrlen);
	if (new_socket < 0) {
		std::cerr << "Listen failed" << std::endl;
		close(server.getSocket());
		exit(1);
	}

	std::cout << "Connected client" << std::endl;

	std::string input;
	while (true) {
		std::getline(std::cin, input);
		if (input == "exit")
			break;
		input = "server " + input + "\n";
		send(new_socket, input.c_str(), input.length(), 0); // Must use poll
	}

	close(new_socket);
	close(server.getSocket());
	return 0;
}