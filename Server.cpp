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

int &Server::getSocket() {
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

void Server::connectUser() {
	int new_socket;
	int addrlen = sizeof(_address);

	// Accept a connection
	new_socket = accept(_server_fd, (struct sockaddr *)&_server_fd, (socklen_t*)&addrlen);
	if (new_socket < 0) {
		std::cerr << "Listen failed" << std::endl;
		close(_server_fd);
		exit(1);
	}
	_user.push_back(User(new_socket, "michel", "michou"));
}
