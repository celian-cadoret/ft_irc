#include "User.hpp"

User::User() : _user_fd(0), _connect_state(0) {}

User::User( User const &src ) : _user_fd(src._user_fd), _username(src._username), _nickname(src._nickname), _connect_state(src._connect_state) {}

User::User( int user_fd ) : _user_fd(user_fd), _connect_state(0) {}

User::~User() {}


int	User::getSocket() {
	return (_user_fd);
}

std::string	User::getUsername() {
	return (_username);
}

std::string	User::getNickname() {
	return (_nickname);
}

int User::getConnectState() {
	return _connect_state;
}

void User::incrementConnectState( int n ) {
	_connect_state += n;
}

void User::setNickname( std::string nickname ) {
	_nickname = nickname;
}

void User::setUsername( std::string username ) {
	_username = username;
}


void User::joinChannel( std::vector<Channel> &channels, std::string name ) {
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->getName() == name)
			it->addUser(*this);
	}
	// Channel was not found
	if (it == channels.end())
		channels.push_back(Channel(name, *this));
}

void User::quitChannel( std::vector<Channel> &channels, std::string name ) {
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->getName() == name)
			it->removeUser(*this);
	}
	// Channel is now empty
	if (it != channels.end() && !it->getUserAmt())
		channels.erase(it);
}


User &User::operator=( User const &src ) {
	_username = src._username;
	_nickname = src._nickname;
	_user_fd = src._user_fd;
	_connect_state = src._connect_state;
	return *this;
}

bool User::operator==( User const &src ) {
	if (_username == src._username && _nickname == src._nickname && _user_fd == src._user_fd)
		return true;
	return false;
}
