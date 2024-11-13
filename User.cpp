#include "User.hpp"

User::User() : _user_fd(0) {}

User::User( User const &src ) : _user_fd(src._user_fd) {}

User::User( int user_fd, std::string username, std::string nickname) : _user_fd(user_fd), _username(username), _nickname(nickname) {}

User::~User() {}


User &User::operator=( User const &src ) {
	if (this != &src) {
		_username = src._username;
		_nickname = src._nickname;
		_user_fd = src._user_fd;
	}
	return *this;
}

int	User::getUserFd() {
	return (_user_fd);
}

std::string	User::getUsername() {
	return (_username);
}


std::string	User::getNickname() {
	return (_nickname);
}
