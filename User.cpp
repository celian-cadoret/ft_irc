#include "User.hpp"

User::User() : _user_fd(0) {}

User::User( User const &src ) : _user_fd(src._user_fd) {}

User::~User() {}


User &User::operator=( User const &src ) {
	if (this != &src) {
		_username = src._username;
		_nickname = src._nickname;
		_user_fd = src._user_fd;
	}
	return *this;
}