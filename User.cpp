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

void User::setSocket( int socket ) {
	_user_fd = socket;
}


bool User::joinChannel( std::vector<Channel> &channels, std::string name ) {
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->getName() == name && !it->isUserInChannel(_nickname)) {
			if (!it->isInviteOnly() || (it->isInviteOnly() && it->isInvited(_nickname))) {
				it->popInvited(_nickname);
				it->addUser(_nickname);
				return true;
			}
			else if (it->isInviteOnly() && !it->isInvited(_nickname))
				return false;
		}
	}
	// Channel was not found
	if (it == channels.end()) {
		channels.push_back(Channel(name, _nickname));
		return true;
	}
	return false;
}

void User::quitChannel( std::vector<Channel> &channels, std::string name ) {
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->getName() == name) {
			it->removeUser(_nickname);
			if (!it->getUserAmt()) {
				std::vector<Channel>::iterator tmp = it - 1;
				channels.erase(it);
				it = tmp;
			}
		}
	}
}


User &User::operator=( User const &src ) {
	_username = src._username;
	_nickname = src._nickname;
	_user_fd = src._user_fd;
	_connect_state = src._connect_state;
	return *this;
}
