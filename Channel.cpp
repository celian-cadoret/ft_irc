#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel( Channel const &src ) : _name(src._name), _user(src._user), _topic(src._topic), _topic_nick(src._topic_nick) {}

Channel::Channel( std::string name, std::string owner ) : _name(name) {
	_user[owner] = true;
}

Channel::~Channel() {}


std::string Channel::getName() {
	return _name;
}

void Channel::addUser( std::string toAdd ) {
	if (_user.find(toAdd) == _user.end())
		_user[toAdd] = false;
}

void Channel::removeUser( std::string toRemove ) {
	if (_user.find(toRemove) != _user.end())
		_user.erase(_user.find(toRemove));
}

void Channel::addOp( std::string toAdd ) {
	if (_user.empty())
		return ;
	
	if (_user.find(toAdd) != _user.end())
		_user[toAdd] = true;
}

void Channel::removeOp( std::string toRemove ) {
	if (_user.empty())
		return ;
	
	if (_user.find(toRemove) != _user.end())
		_user[toRemove] = false;
}

int Channel::getUserAmt() {
	return _user.size();
}

std::string Channel::getUserList() {
	if (_user.empty())
		return "";

	std::string out;
	for (std::map<std::string, bool>::iterator it = _user.begin(); it != _user.end(); it++) {
		if (it->second)
			out += "@";
		out += it->first;
		out += " ";
	}
	return out.substr(0, out.size() - 1);
}

std::map<std::string, bool> &Channel::getUsers() {
	return _user;
}

void Channel::setTopic( std::string topic, std::string nick )  {
	_topic = topic;
	_topic_nick = nick;
}

std::string Channel::getTopic() {
	return _topic;
}

std::string Channel::getTopicNick() {
	return _topic_nick;
}


bool Channel::isUserInChannel( std::string nickname ) {
	if (_user.find(nickname) == _user.end())
		return false;
	return true;
}

bool Channel::isUserOp( std::string nickname ) {
	if (_user.empty())
		return false;

	if (_user.find(nickname) != _user.end())
		return _user[nickname];

	return false;
}


Channel &Channel::operator=( Channel const &src ) {
	_name = src._name;
	_user = src._user;
	_topic = src._topic;
	_topic_nick = src._topic_nick;
	return *this;
}