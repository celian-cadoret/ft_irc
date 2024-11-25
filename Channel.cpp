#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel( Channel const &src ) : _name(src._name), _user(src._user) {}

Channel::Channel( std::string name, std::string owner ) : _name(name) {
	_user[owner] = true;
	_op.push_back(owner);
}

Channel::~Channel() {}


std::string Channel::getName() {
	return _name;
}

void Channel::addUser( std::string toAdd ) {
	if (_user.find(toAdd) == _user.end())
		_user[toAdd] = true;
}

void Channel::removeUser( std::string toRemove ) {
	if (_user.find(toRemove) != _user.end())
		_user.erase(_user.find(toRemove));

	if (_op.empty())
		return ;

	for (std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); it++) {
		if (*it == toRemove)
			_op.erase(it);
	}
}

void Channel::addOp( std::string toAdd ) {
	if (_op.empty())
		return ;
	
	for (std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); it++) {
		if (*it == toAdd)
			return ;
	}
	_op.push_back(toAdd);
}

void Channel::removeOp( std::string toRemove ) {
	if (_op.empty())
		return ;
	
	for (std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); it++) {
		if (*it == toRemove)
			_op.erase(it);
	}
}

void Channel::setUserOnline( std::string nickname ) {
	if (_user.empty())
		return ;
	
	if (_user.find(nickname) != _user.end())
		_user[nickname] = true;
}

void Channel::setUserOffline( std::string nickname ) {
	if (_user.empty())
		return ;
	
	if (_user.find(nickname) != _user.end())
		_user[nickname] = false;
}

int Channel::getUserAmt() {
	return _user.size();
}

std::string Channel::getUserList() {
	if (_user.empty())
		return "";

	std::string out;
	for (std::map<std::string, bool>::iterator it = _user.begin(); it != _user.end(); it++) {
		if (isUserOp(it->first))
			out += "@";
		out += it->first;
		out += " ";
	}
	return out.substr(0, out.size() - 1);
}

std::map<std::string, bool> &Channel::getUsers() {
	return _user;
}

bool Channel::getUserState( std::string nickname ) {
	return _user[nickname];
}

bool Channel::isUserInChannel( std::string nickname ) {
	if (_user.find(nickname) == _user.end())
		return false;
	return true;
}

bool Channel::isUserOp( std::string nickname ) {
	if (_op.empty())
		return false;

	for (std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); it++) {
		if (*it == nickname)
			return true;
	}
	return false;
}


Channel &Channel::operator=( Channel const &src ) {
	_name = src._name;
	_user = src._user;
	return *this;
}