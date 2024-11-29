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

int Server::getSocketFromNickname( std::string nickname ) {
	std::vector<User>::iterator it;
	for (it = _user.begin(); it != _user.end(); it++) {
		if (it->getNickname() == nickname)
			return it->getSocket();
	}
	return 0;
}

Channel *Server::getChannel( std::string name ) {
	if (_channels.empty())
		return NULL;

	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if (it->getName() == name)
			return &(*it);
	}
	return NULL;
}

bool Server::isRunning() {
	return _running;
}


int Server::start() {
	// Create socket (fd)
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (!_server_fd) {
		std::cerr << "[SERVER] Socket failed!" << std::endl;
		return 1;
	}
	// Set its options
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::cerr << "[SERVER] Setsockopt failed!" << std::endl;
		close(_server_fd);
		return 1;
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
	// Associate socket to address and port
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "[SERVER] Bind failed!" << std::endl;
		close(_server_fd);
		return 1;
	}
	// Listen socket
	if (listen(_server_fd, 3) < 0) {
		std::cerr << "[SERVER] Listen failed!" << std::endl;
		close(_server_fd);
		return 1;
	}
	_running = true;
	std::cout << "[SERVER] Server \"" << _name << "\" running on port " << _port << "." << std::endl;
	return 0;
}

void Server::stop() {
	_running = false;
}

void Server::connectUser( std::vector<pollfd> &new_pollfds ) {
	int addrlen = sizeof(_address);
	int new_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
	if (new_socket < 0) {
		std::cerr << "[SERVER] Client listen failed" << std::endl;
		return ;
	}
	_user.push_back(User(new_socket));
	std::cout << "[SERVER] Negotiating with client #" << _user.size() << ", fd=" << new_socket << std::endl;

	pollfd new_client = {new_socket, POLLIN, 0};
	new_pollfds.push_back(new_client);
}

void Server::manageUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	size_t buffer_size = 2048;
	char buff[buffer_size];

	int rc = recv(it->fd, buff, buffer_size - 1, 0); // TODO
	buff[rc] = '\0';
	if (rc < 0) {
		std::cerr << "Read error" << std::endl;
		return ;
	}
	if (rc == 0) {
		User &curr = _user[getUserFromSocket(it->fd)];
		std::cout << "[" << it->fd << "<" << curr.getNickname() << ">] Client disconnected" << std::endl;
		it = deleteUser(pollfds, it);
		updateUserList(_user[getUserFromSocket(it->fd)].getNickname());
	}
	else {
		std::string msg = buff;
		User &curr = _user[getUserFromSocket(it->fd)];

		std::cout << "[" << it->fd << "<" << curr.getNickname() << ">] " << msg;
		if (curr.getConnectState() > 1)
			parseMessage(it, pollfds, msg);
		else
			treatRequests(msg, curr);
	}
}

void Server::treatRequests( std::string msg, User &curr ) {
	std::string content;
	if (msg.substr(0, 4) == "CAP ") {} // ignore
	if (msg.substr(0, 6) == "PASS :") {
		content = msg.substr(msg.find(":") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);
	
		if (content != _password)
			throw ReqInvalidPass();
		curr.incrementConnectState();
	}
	if (msg.substr(0, 5) == "NICK ") {
		content = msg.substr(msg.find(" ") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);

		if (content.find(" ") != std::string::npos)
			throw ReqNickSpace();
		for (std::vector<User>::iterator it = _user.begin(); it != _user.end(); it++) {
			if (it->getNickname() == content)
				throw ReqNickExists();
		}
		curr.setNickname(content);
	}
	if (msg.substr(0, 5) == "USER ") {
		if (msg.find(":") == std::string::npos)
			throw std::exception();
		
		content = msg.substr(msg.find(":") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);

		if (!curr.getConnectState())
			throw ReqInvalidPass();
		curr.setUsername(content);
		curr.incrementConnectState();
		msg = "Welcome Successfully connected to " + _name + " !\r\n";
		send(curr.getSocket(), msg.c_str(), msg.size(), 0);
		std::cout << "[SERVER] User successfully connected: " << curr.getNickname() << ", " << curr.getUsername() << std::endl;
	}
}

void Server::parseMessage( std::vector<pollfd>::iterator &it, std::vector<pollfd> &pollfds, std::string msg ) {
	User &curr = _user[getUserFromSocket(it->fd)];
	std::string curr_user = curr.getNickname();
	std::string channel_name, target;
	std::vector<std::string> args;

	if (msg.substr(0, 5) == "JOIN ") {
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		curr.joinChannel(_channels, channel_name);
		joinChannelClient(it, channel_name);

	}
	else if (msg.substr(0, 6) == "QUIT :") { // Leaving server
		it = deleteUser(pollfds, it);
		updateUserList();
	}
	else if (msg.substr(0, 6) == "PART #") { // Leaving a channel
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		if (!getChannel(channel_name))
			return ;
		curr.quitChannel(_channels, channel_name);
		updateUserList();
	}
	else if (msg.substr(0, 9) == "PRIVMSG #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (!getChannel(channel_name) || !getChannel(channel_name)->isUserInChannel(curr_user)) {
			curr.joinChannel(_channels, channel_name);
			joinChannelClient(it, channel_name);
		}
		msg = ":" + curr_user + " " + msg;
		sendAll(msg, it->fd);

	}
	else if (msg.substr(0, 6) == "KICK #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];
		target = args[2];

		if (!getChannel(channel_name))
			return ;

		if (!getChannel(channel_name)->isUserInChannel(target)) {
			msg = "Error " + target + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (getChannel(channel_name)->isUserOp(curr_user)) {
			msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
			curr.quitChannel(_channels, channel_name);
			sendAll(msg);
		}
		else {
			msg = "Error You need to be a channel operator in " + channel_name + " to do that.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}

	}
	else if (msg.substr(0, 7) == "TOPIC #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];
		target = "";
		if (args.size() > 2) {
			for (std::vector<std::string>::iterator its = args.begin() + 2; its != args.end(); its++) {
				target += *its;
				target += " ";
			}
		}
		if (target != "" && (target[target.size() - 1] == ' ' || target[target.size() - 1] == '\n'))
			target = target.substr(0, target.size() - 1);
		if (target != "" && (target[target.size() - 1] == ' ' || target[target.size() - 1] == '\n'))
			target = target.substr(0, target.size() - 1);
		if (target != "" && target[0] == ':')
			target = target.substr(1);
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		
		if (!getChannel(channel_name))
			return ;

		if (args.size() < 3) {
			if (getChannel(channel_name)->getTopic() != "")
				msg = ":" + getChannel(channel_name)->getTopicNick() + " 332 " + curr_user + " " + channel_name + " :" + getChannel(channel_name)->getTopic() + "\r\n";
			else
				msg = "TOPIC The channel " + channel_name + " has no topic set.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else {
			if (getChannel(channel_name)->isUserOp(curr_user)) {
				getChannel(channel_name)->setTopic(target, curr_user + "!~" + curr_user + "@localhost");
				msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
				sendAll(msg);
			}
			else {
				msg = "Error You need to be a channel operator in " + channel_name + " to do that.\r\n";
				send(it->fd, msg.c_str(), msg.size(), 0);
			}
		}	
	}
	else if (msg.substr(0, 7) == "INVITE ") {
		args = splitStr(msg, ' ');
		target = args[1];
		channel_name = args[2];
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		if (!getSocketFromNickname(target)) {
			msg = "Error " + target + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (!getChannel(channel_name)) {
			msg = "Error " + channel_name + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else {
			if (getChannel(channel_name)->isUserInChannel(target)) {
				msg = "443 " + curr_user + " " + target + " " + channel_name + " is already on channel\r\n";
				send(it->fd, msg.c_str(), msg.size(), 0);
			}
			else {
				msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
				send(getSocketFromNickname(target), msg.c_str(), msg.size(), 0);
			}
		}
	}
	else if (toLowerStr(msg.substr(0, 5)) == "mode ") {
		bool positive = true;
		args = splitStr(msg, ' ');

		if (args.size() == 1) {
			msg = "Error MODE: This command requires more parameters.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
			return ;
		}

		channel_name = args[1];
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);

		if (!getChannel(channel_name)) {
			msg = "Error " + channel_name + ": No such channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (args.size() < 3) {
			// afficher modes
		}
		else {
			std::string flags = args[2];
			if (flags[flags.size() - 1] == '\n')
			flags = flags.substr(0, flags.size() - 1);

			size_t i = 0;
			while (flags[i] == '+' || flags[i] == '-') {
				if (flags[i] == '+')
					positive = true;
				else
					positive = false;
				i++;
			}
			while (i < flags.size()) {
				if (flags[i] != 'i' && flags[i] != 't' && flags[i] != 'k' && flags[i] != 'o' && flags[i] != 'l') {
					msg = "472 tgriblin ";
					msg += flags[i];
					msg += " is an unknown mode char to me\r\n";
					send(it->fd, msg.c_str(), msg.size(), 0);
					return ;
				}
				//if (flags[i] == 'i')
				i++;
			}
		}
	}
	else if (msg == "exit\n" || msg == "shutdown\n")
		stop();
	// 
	// 
	// MODE : fournir les flags avec +/- en un seul arg (-ok ou +ikt par exemple)
	// ARGS DE MODE : /mode <channel> <flags> <args des flags>
}

std::vector<pollfd>::iterator Server::deleteUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it ) {
	if (!_channels.empty()) {
		for (std::vector<Channel>::iterator itc = _channels.begin(); itc != _channels.end(); itc++) {
			itc->removeUser(_user[getUserFromSocket(it->fd)].getNickname());
			if (!itc->getUserAmt()) {
				std::vector<Channel>::iterator tmp = itc - 1;
				_channels.erase(itc);
				itc = tmp;
				continue;
			}
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
	if (getChannel(name)->getTopic() != "") {
		msg = ":" + getChannel(name)->getTopicNick() + " TOPIC " + name + " :" + getChannel(name)->getTopic() + "\r\n";
		send(it->fd, msg.c_str(), msg.size(), 0);
	}
	updateUserList(name);
	
}

void Server::updateUserList( std::string channel ) {
	if (_channels.empty())
		return ;

	std::string msg;
	if (channel == "") {
		std::vector<Channel>::iterator it;
		for (it = _channels.begin(); it != _channels.end(); it++) {
			updateUserList(it->getName());
		}
	}
	else {
		if (!getChannel(channel))
			return ;
		std::map<std::string, bool>::iterator it;
		for (it = getChannel(channel)->getUsers().begin(); it != getChannel(channel)->getUsers().end(); it++) {
			msg = ":" + _name + " 353 " + it->first + " = " + channel + " :" + getChannel(channel)->getUserList() + "\r\n";
			sendAll(msg);
			//msg = ":" + _name + " 353 " + it->first + " " + channel + " :End of NAMES list.\r\n";
			//sendAll(msg);
		}
	}
}


void Server::sendAll( std::string buff, int ignore ) {
	for (size_t i = 0; i < _user.size(); i++) {
		if (_user[i].getSocket() != ignore)
			send(_user[i].getSocket(), buff.c_str(), buff.size(), 0);
	}
}

void Server::sendAllChannel( std::string channel, std::string buff, int ignore ) {
	if (!getChannel(channel))
		return ;
	
	std::map<std::string, bool>::iterator it;
	for (it = getChannel(channel)->getUsers().begin(); it != getChannel(channel)->getUsers().end(); it++) {
		int fd = getSocketFromNickname(it->first);
		if (!fd || fd == ignore)
			continue;
		send(fd, buff.c_str(), buff.size(), 0);
	}
}


const char *Server::ReqInvalidPass::what() const throw() {
	return "Invalid password.";
}

const char *Server::ReqNickSpace::what() const throw() {
	return "Nickname cannot contain spaces.";
}

const char *Server::ReqNickExists::what() const throw() {
	return "Nickname already used.";
}