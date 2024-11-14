#include "User.hpp"

User::User() : _user_fd(0), _op(false), _username_set(false), _nickname_set(false), _password_set(false) {}

User::User( User const &src ) : _user_fd(src._user_fd), _op(src._op), _username_set(src._username_set), _nickname_set(src._nickname_set), _password_set(src._password_set) {}

User::User( int user_fd ) : _user_fd(user_fd), _op(false), _username_set(false), _nickname_set(false), _password_set(false) {}

User::~User() {}


User &User::operator=( User const &src ) {
	_username = src._username;
	_nickname = src._nickname;
	_user_fd = src._user_fd;
	_password = src._password;
	_username_set = src._username_set;
	_nickname_set = src._nickname_set;
	_password_set = src._password_set;
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

std::string User::getPassword() {
	return _password;
}

void User::setUsername( std::string username ) {
	if (!_username_set)
		_username = username;
	_username_set = true;
}

void User::setNickname( std::string nickname ) {
	if (!_nickname_set)
		_nickname = nickname;
	_nickname_set = true;
}

void User::setPassword( std::string password ) {
	if (!_password_set)
		_password = password;
	_password_set = true;
}
