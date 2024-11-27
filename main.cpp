#include "main.hpp"

int main( int ac, char **av ) {
	if (ac != 3) {
		std::cerr << "Usage:  ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	Server server("PortHub", atoi(av[1]), av[2]);
	if (server.start())
		return 1;

	int error = 0;

	std::vector<pollfd> pollfds;
	pollfd tmp = {server.getSocket(), POLLIN, 0};
	pollfds.push_back(tmp);

	std::string input;
	while (true) {
		std::vector<pollfd> new_pollfds;

		if (poll(pollfds.data(), pollfds.size(), -1) < 0) {
			error = 1;
			break;
		}

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
						it = server.deleteUser(pollfds, it);
						continue;
					}
				}
			}
		}
		pollfds.insert(pollfds.end(), new_pollfds.begin(), new_pollfds.end());
	}

	for (int i = 0; i < server.getUserAmt(); i++) {
		close(server.getUser(i).getSocket());
	}
	close(server.getSocket());

	return error;
}