#include "main.hpp"

int main( int ac, char **av ) {
	if (ac != 3) {
		std::cerr << "Usage:  ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	Server server(atoi(av[1]), av[2]);
	server.start();

	std::vector<pollfd> pollfds;
	pollfd tmp = {server.getSocket(), POLLIN, 0};
	pollfds.push_back(tmp);

	std::string input;
	while (true) {
		std::vector<pollfd> new_pollfds;

		poll(pollfds.data(), pollfds.size(), -1);

		for (std::vector<struct pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); it++) {
			if (it->revents & POLLIN) {
				if (it->fd == server.getSocket())
					server.connectUser(new_pollfds);
				else
					server.manageUser(pollfds, it);
			}
			pollfds.insert(pollfds.end(), new_pollfds.begin(), new_pollfds.end());
		}





		/*std::getline(std::cin, input);
		if (input == "exit")
			break;
		input = "server " + input + "\n";
		for (int i = 0; i < server.getUserAmt(); i++) {
			send(server.getUser(i).getSocket(), input.c_str(), input.length(), 0); // Must use poll
		}*/
	}

	for (int i = 0; i < server.getUserAmt(); i++) {
		close(server.getUser(i).getSocket());
	}
	close(server.getSocket());
	return 0;
}