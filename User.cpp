#include "User.hpp"

User::User() : _user_fd(0), _op(false), _connect_state(0) {}

User::User( User const &src ) : _user_fd(src._user_fd), _username(src._username), _nickname(src._nickname), _op(src._op), _connect_state(src._connect_state) {}

User::User( int user_fd ) : _user_fd(user_fd), _op(false), _connect_state(0) {}

User::~User() {}


User &User::operator=( User const &src ) {
	_username = src._username;
	_nickname = src._nickname;
	_user_fd = src._user_fd;
	_connect_state = src._connect_state;
	return *this;
}

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
