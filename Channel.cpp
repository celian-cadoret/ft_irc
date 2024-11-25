#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel( Channel const &src ) : _name(src._name), _user(src._user) {}

Channel::Channel( std::string name, std::string owner ) : _name(name) {
	bool tmp[2] = {true, true};
	_user[owner] = tmp;
}

Channel::~Channel() {}


std::string Channel::getName() {
	return _name;
}

void Channel::addUser( std::string toAdd ) {
	bool tmp[2] = {true, false};
	if (_user.find(toAdd) == _user.end())
		_user[toAdd] = tmp;
}

void Channel::removeUser( std::string toRemove ) {
	if (_user.find(toRemove) != _user.end())
		_user.erase(_user.find(toRemove));
}

void Channel::addOp( std::string toAdd ) {
	if (_user.find(toAdd) != _user.end())
		_user[toAdd][1] = false;
}

void Channel::removeOp( std::string toRemove ) {
	if (_user.find(toRemove) != _user.end())
		_user[toRemove][1] = false;
}

void Channel::setUserOnline( std::string nickname ) {
	if (_user.find(nickname) != _user.end())
		_user[nickname][0] = true;
}

void Channel::setUserOffline( std::string nickname ) {
	if (_user.find(nickname) != _user.end())
		_user[nickname][0] = false;
}

int Channel::getUserAmt() {
	return _user.size();	
}

std::string Channel::getUserList() {
	std::string out;
	for (std::map<std::string, bool*>::iterator it = _user.begin(); it != _user.end(); it++) {
		out += it->first;
		out += " ";
	}
	return out.substr(0, out.size() - 1);
}

std::map<std::string, bool*> &Channel::getUsers() {
	return _user;
}

bool Channel::getUserState( std::string nickname ) {
	return _user[nickname][0];
}

bool Channel::isUserInChannel( std::string nickname ) {
	if (_user.find(nickname) != _user.end())
		return true;
	return false;
}


Channel &Channel::operator=( Channel const &src ) {
	_name = src._name;
	_user = src._user;
	return *this;
}