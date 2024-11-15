#include "Server.hpp"

Server::Server() : _port(0), _server_fd(0) {}

Server::Server( Server const &src ) : _port(src._port), _password(src._password), _server_fd(src._server_fd) {}

Server::Server( int port, std::string password ) : _port(port), _password(password), _server_fd(0) {}

Server::~Server() {}


Server &Server::operator=( Server const &src ) {
	_port = src._port;
	_password = src._password;
	_server_fd = src._server_fd;
	return *this;
}


int Server::getPort() {
	return _port;
}

std::string Server::getPassword() {
	return _password;
}

int Server::getSocket() {
	return _server_fd;
}

struct sockaddr_in &Server::getAddress() {
	return _address;
}

User &Server::getUser( int i ) {
	return _user[i];
}

int Server::getUserAmt() {
	return _user.size();
}

int Server::getUserFromSocket( int socket ) {
	for (size_t i = 0; i < _user.size(); i++) {
		if (_user[i].getSocket() == socket)
			return i;
	}
	return 0;
}


void Server::start() {
	int opt = 1;

	// Create socket
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (!_server_fd) {
		std::cerr << "Socket failed" << std::endl;
		exit(1);
	}

	// Attack socket to the provided port
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::cerr << "Setsockopt error" << std::endl;
		close(_server_fd);
		exit(1);
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);

	// Associate socket to address and port
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "Bind failed" << std::endl;
		close(_server_fd);
		exit(1);
	}

	// Listen socket
	if (listen(_server_fd, 3) < 0) {
		std::cerr << "Listen failed" << std::endl;
		close(_server_fd);
		exit(1);
	}
}

void Server::connectUser( std::vector<pollfd> &new_pollfds ) {
	int addrlen = sizeof(_address);
	int new_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
	if (new_socket < 0) {
		std::cerr << "Listen failed" << std::endl;
		close(_server_fd);
		exit(1);
	}
	_user.push_back(User(new_socket));
	std::cout << "Connected client " << _user.size() << " (" << new_socket << ")" << std::endl;

	pollfd new_client = {new_socket, POLLIN | POLLPRI, 0};
	new_pollfds.push_back(new_client);
}

void Server::manageUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	char buff[2048];

	int rc = recv(it->fd, buff, 2048, 0);
	buff[rc] = '\0';
	if (rc < 0) {
		std::cerr << "Read error" << std::endl;
		return ;
	}
	if (rc == 0) {
		std::cout << "User left" << std::endl;
		it = deleteUser(pollfds, it);
	}
	else {
		std::string msg;
		msg.assign(buff);
		std::string userenv = std::getenv("USER");
		User &curr = _user[getUserFromSocket(it->fd)];

		if (curr.getConnectState() == 0) // Ignore the CAP LS
			curr.incrementConnectState();
		else if (curr.getConnectState() == 1) {
			if (msg.substr(0, 5) == "NICK ")
				throw std::exception(); // empty
			if (msg.substr(0, 6) == "PASS :") {
				if (msg.substr(6, msg.size() - 7) == _password)
					curr.incrementConnectState();
				else 
					throw std::exception(); // invalid
			}
			
		}
		else if (curr.getConnectState() == 2) {
			if (msg.substr(0, 5) == "NICK ") {
				curr.setNickname(msg.substr(5, msg.size() - 6));
				curr.incrementConnectState();
			}
		}
		else if (curr.getConnectState() == 3) {
			if (msg.substr(0, 5) == "USER ") {
				curr.setUsername(msg.substr(11 + userenv.size(), msg.size() - 12 - userenv.size()));
				curr.incrementConnectState();
				std::cout << "User successfully connected" << std::endl;
				std::cout << curr.getNickname() << ", " << curr.getUsername() << std::endl;
			}
		}
		else if (msg.substr(0, 6) != "QUIT :") {
			msg = curr.getNickname() + " " + msg;
			for (std::vector<pollfd>::iterator it3 = pollfds.begin() + 1; it3 != pollfds.end(); it3++) {
				send(it3->fd, msg.c_str(), msg.size(), 0);
			}
		}
		//std::cout << msg;
	}
}

std::vector<pollfd>::iterator Server::deleteUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	_user.erase(_user.begin() + getUserFromSocket(it->fd));
	close(it->fd);
	std::vector<pollfd>::iterator its = it - 1;
	pollfds.erase(it);
	return its;
}