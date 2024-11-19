#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel( Channel const &src ) : _name(src._name), _users(src._users), _op(src._op) {}

Channel::Channel( std::string name, User &owner ) : _name(name) {
	_users.push_back(owner);
	_op.push_back(owner);
}

Channel::~Channel() {}


std::string Channel::getName() {
	return _name;
}

void Channel::addUser( User &toAdd ) {
	_users.push_back(toAdd);
}

void Channel::removeUser( User &toRemove ) {
	for (std::vector<User>::iterator it = _users.begin(); it != _users.end(); it++) {
		if (*it == toRemove)
			_users.erase(it);
	}
}

void Channel::addOp( User &toAdd ) {
	_op.push_back(toAdd);
}

void Channel::removeOp( User &toRemove ) {
	for (std::vector<User>::iterator it = _op.begin(); it != _op.end(); it++) {
		if (*it == toRemove)
			_op.erase(it);
	}
}

int Channel::getUserAmt() {
	return _users.size();	
}


Channel &Channel::operator=( Channel const &src ) {
	_name = src._name;
	_users = src._users;
	_op = src._op;
	return *this;
}