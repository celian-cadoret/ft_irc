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

		std::vector<pollfd>::iterator it;
		for (it = pollfds.begin(); it != pollfds.end(); it++) {
			if (it->revents & POLLIN) {
				if (it->fd == server.getSocket())
					server.connectUser(new_pollfds);
				else {
					try {
						server.manageUser(pollfds, it);
					}
					catch (std::exception &e) {
						std::cerr << e.what() << std::endl;
						if (it + 1 == pollfds.end()) {
							std::vector<pollfd>::iterator ittmp = it - 1;
							server.deleteUser(pollfds, it);
							it = ittmp;
							continue;
						}
						else
							server.deleteUser(pollfds, it);
					}
				}
			}
		}
		pollfds.insert(pollfds.end(), new_pollfds.begin(), new_pollfds.end());
		//send(server.getUser(0).getSocket(), input.c_str(), input.length(), 0);
	}

	for (int i = 0; i < server.getUserAmt(); i++) {
		close(server.getUser(i).getSocket());
	}
	close(server.getSocket());
	return 0;
}