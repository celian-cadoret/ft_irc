#include "Server.hpp"

Server::Server() : _port(0), _server_fd(0) {}

Server::Server( Server const &src ) : _port(src._port), _password(src._password), _server_fd(src._server_fd) {}

Server::Server( std::string name, int port, std::string password ) : _name(name), _port(port), _password(password), _server_fd(0) {}

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

Channel *Server::getChannel( std::string name ) {
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if (it->getName() == name)
			return &(*it);
	}
	return NULL;
}


void Server::start() {
	// Create socket
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (!_server_fd)
		throw std::exception(); // Socket failed

	// Attack socket to the provided port
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		throw std::exception(); // Setsockopt failed
		close(_server_fd);
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);

	// Associate socket to address and port
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		throw std::exception(); // Bind failed
		close(_server_fd);
	}

	// Listen socket
	if (listen(_server_fd, 3) < 0) {
		throw std::exception(); // Listen failed
		close(_server_fd);
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
	std::cout << "Negotiating with client #" << _user.size() << ", fd=" << new_socket << std::endl;

	pollfd new_client = {new_socket, POLLIN | POLLOUT, 0};
	new_pollfds.push_back(new_client);
}

void Server::manageUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	char buff[8192];

	int rc = recv(it->fd, buff, 8192, 0); // TODO
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
		std::string msg = buff;
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
				std::cout << "User successfully connected: " << curr.getNickname() << ", " << curr.getUsername() << std::endl;
			}
		}
		else if (msg.substr(0, 6) != "QUIT :") {
			if (msg.substr(0, 5) == "JOIN ") {
				std::string channel_name = msg.substr(5);
				std::string curr_user = _user[getUserFromSocket(it->fd)].getNickname();
				if (channel_name[channel_name.size() - 1] == '\n')
					channel_name = channel_name.substr(0, channel_name.size() - 1);
				curr.joinChannel(_channels, channel_name);
				joinChannelClient(it, channel_name);
			}
			else if (msg.substr(0, 9) == "PRIVMSG #" && msg.find(':') != std::string::npos) {
				std::string channel_name = msg.substr(8, msg.find(':') - 9);
				std::string curr_user = _user[getUserFromSocket(it->fd)].getNickname();
				if (!getChannel(channel_name) || !getChannel(channel_name)->isUserInChannel(curr_user)) {
					curr.joinChannel(_channels, channel_name);
					joinChannelClient(it, channel_name);
				}
				msg = ":" + curr_user + " " + msg;
				sendAll(msg, it->fd);	
			}
		}
	}
}

std::vector<pollfd>::iterator Server::deleteUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	if (!_channels.empty()) {
		for (std::vector<Channel>::iterator itc = _channels.begin(); itc != _channels.end(); itc++) {
			itc->removeUser(_user[getUserFromSocket(it->fd)].getNickname());
		}
	}
	_user.erase(_user.begin() + getUserFromSocket(it->fd));
	close(it->fd);
	std::vector<pollfd>::iterator its = it - 1;
	pollfds.erase(it);
	return its;
}

void Server::joinChannelClient( std::vector<pollfd>::iterator &it, std::string name ) {
	if (!getChannel(name))
		return ;
	
	std::string msg;
	User &curr = _user[getUserFromSocket(it->fd)];

	msg = ":" + curr.getNickname() + "!~" + curr.getNickname() + "@localhost JOIN " + name + "\r\n";
	sendAll(msg);
	msg = ":" + _name + " MODE " + name + " +nt\r\n";
	send(it->fd, msg.c_str(), msg.size(), 0);
	msg = ":" + _name + " 332 " + curr.getNickname() + " " + name + " :Goat land le lieu des goats\r\n";
	send(it->fd, msg.c_str(), msg.size(), 0);
	msg = ":" + _name + " 353 " + curr.getNickname() + " = " + name + " :" + getChannel(name)->getUserList() + "\r\n";
	send(it->fd, msg.c_str(), msg.size(), 0);
	msg = ":" + _name + " 353 " + curr.getNickname() + " " + name + " :End of NAMES list.\r\n";
	send(it->fd, msg.c_str(), msg.size(), 0);
}


void Server::sendAll( std::string buff, int ignore ) {
	for (size_t i = 0; i < _user.size(); i++) {
		if (_user[i].getSocket() != ignore)
			send(_user[i].getSocket(), buff.c_str(), buff.size(), 0);
	}
}