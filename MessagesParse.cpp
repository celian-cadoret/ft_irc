#include "main.hpp"

void Server::treatRequests( std::string msg, User &curr ) {
	std::string content;
	if (msg.substr(0, 4) == "CAP ") {} // ignore
	if (msg.substr(0, 6) == "PASS :") {
		content = msg.substr(msg.find(":") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);
	
		if (content != _password)
			throw ReqInvalidPass();
		curr.setReqState('p');
	}
	if (msg.substr(0, 5) == "NICK ") {
		content = msg.substr(msg.find(" ") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);

		if (content.find(" ") != std::string::npos)
			throw ReqNickSpace();
		for (std::vector<User>::iterator it = _user.begin(); it != _user.end(); it++) {
			if (it->getNickname() == content)
				throw ReqNickExists();
		}
		curr.setNickname(content);
		curr.setReqState('n');
	}
	if (msg.substr(0, 5) == "USER ") {
		if (msg.find(":") == std::string::npos)
			throw std::exception();
		
		content = msg.substr(msg.find(":") + 1);
		if (content[content.size() - 1] == '\n')
			content = content.substr(0, content.size() - 1);

		curr.setUsername(content);
		curr.setReqState('u');
	}
	if (curr.isConnected()) {
		if (!curr.isPassSet())
			throw ReqInvalidPass();
		msg = "Welcome Successfully connected to " + _name + " !\r\n";
		send(curr.getSocket(), msg.c_str(), msg.size(), 0);
		std::cout << "[SERVER] User successfully connected: " << curr.getNickname() << ", " << curr.getUsername() << std::endl;
	}
}

void Server::parseMessage( std::vector<pollfd>::iterator &it, std::vector<pollfd> &pollfds, std::string msg ) {
	User &curr = _user[getUserFromSocket(it->fd)];
	std::string curr_user = curr.getNickname();
	std::string channel_name, target;
	std::vector<std::string> args;

	if (msg.substr(0, 5) == "JOIN ") {
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		if (curr.joinChannel(_channels, channel_name))
			joinChannelClient(it, channel_name);
		else {
			msg = "473 " + curr_user + " " + channel_name + " Cannot join channel (+i) - you must be invited\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
	}

	else if (msg.substr(0, 6) == "QUIT :") { // Leaving server
		it = deleteUser(pollfds, it);
		msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
		sendAll(msg);
	}

	else if (msg.substr(0, 6) == "PART #") { // Leaving a channel
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		if (!getChannel(channel_name))
			return ;
		curr.quitChannel(_channels, channel_name);
		msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
		sendAll(msg);
	}

	else if (msg.substr(0, 9) == "PRIVMSG #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];

		if (!getChannel(channel_name) || !getChannel(channel_name)->isUserInChannel(curr_user)) {
			if (curr.joinChannel(_channels, channel_name)) {
				joinChannelClient(it, channel_name);
			}
			else {
				msg = "404 " + curr_user + " " + channel_name + " Cannot send to nick/channel\r\n";
				send(it->fd, msg.c_str(), msg.size(), 0);
				return ;
			}
		}
		msg = ":" + curr_user + " " + msg;
		sendAll(msg, it->fd);

	}

	else if (msg.substr(0, 6) == "KICK #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];
		target = args[2];

		if (!getChannel(channel_name))
			return ;

		if (!getChannel(channel_name)->isUserInChannel(target)) {
			msg = "Error " + target + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (getChannel(channel_name)->isUserOp(curr_user)) {
			User &target_user = _user[getUserFromSocket(getSocketFromNickname(target))];
			target_user.quitChannel(_channels, channel_name);
			msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
			sendAll(msg);
		}
		else {
			msg = "Error You need to be a channel operator in " + channel_name + " to do that.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}

	}

	else if (msg.substr(0, 7) == "TOPIC #") {
		args = splitStr(msg, ' ');
		channel_name = args[1];
		target = "";
		if (args.size() > 2) {
			for (std::vector<std::string>::iterator its = args.begin() + 2; its != args.end(); its++) {
				target += *its;
				target += " ";
			}
		}
		if (target != "" && (target[target.size() - 1] == ' ' || target[target.size() - 1] == '\n'))
			target = target.substr(0, target.size() - 1);
		if (target != "" && (target[target.size() - 1] == ' ' || target[target.size() - 1] == '\n'))
			target = target.substr(0, target.size() - 1);
		if (target != "" && target[0] == ':')
			target = target.substr(1);
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		
		if (!getChannel(channel_name))
			return ;

		if (args.size() < 3) {
			if (getChannel(channel_name)->getTopic() != "")
				msg = ":" + getChannel(channel_name)->getTopicNick() + " 332 " + curr_user + " " + channel_name + " :" + getChannel(channel_name)->getTopic() + "\r\n";
			else
				msg = "TOPIC The channel " + channel_name + " has no topic set.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else {
			if (getChannel(channel_name)->isUserOp(curr_user) || !getChannel(channel_name)->isTopicRestricted()) {
				getChannel(channel_name)->setTopic(target, curr_user + "!~" + curr_user + "@localhost");
				msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
				sendAll(msg);
			}
			else {
				msg = "Error You need to be a channel operator in " + channel_name + " to do that.\r\n";
				send(it->fd, msg.c_str(), msg.size(), 0);
			}
		}	
	}
	
	else if (msg.substr(0, 7) == "INVITE ") {
		args = splitStr(msg, ' ');
		target = args[1];
		channel_name = args[2];
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);
		if (!getSocketFromNickname(target)) {
			msg = "Error " + target + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (!getChannel(channel_name)) {
			msg = "Error " + channel_name + ": No such nick/channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else {
			if (getChannel(channel_name)->isUserInChannel(target)) {
				msg = "443 " + curr_user + " " + target + " " + channel_name + " is already on channel\r\n";
				send(it->fd, msg.c_str(), msg.size(), 0);
			}
			else {
				getChannel(channel_name)->addInvited(target);
				msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
				send(getSocketFromNickname(target), msg.c_str(), msg.size(), 0);
			}
		}
	}

	else if (toLowerStr(msg.substr(0, 5)) == "mode ") {
		bool positive = true;
		args = splitStr(msg, ' ');

		if (args.size() == 1) {
			msg = "Error MODE: This command requires more parameters.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
			return ;
		}

		channel_name = args[1];
		if (channel_name[channel_name.size() - 1] == '\n')
			channel_name = channel_name.substr(0, channel_name.size() - 1);

		if (!getChannel(channel_name)) {
			msg = "Error " + channel_name + ": No such channel.\r\n";
			send(it->fd, msg.c_str(), msg.size(), 0);
		}
		else if (args.size() < 3) {
			// afficher modes
		}
		else {
			std::string flags = args[2];
			if (flags[flags.size() - 1] == '\n')
			flags = flags.substr(0, flags.size() - 1);

			size_t i = 0;
			while (flags[i] == '+' || flags[i] == '-') {
				if (flags[i] == '+')
					positive = true;
				else
					positive = false;
				i++;
			}
			size_t curr_arg = 3;
			std::string tmp_arg;
			while (i < flags.size()) {
				if (flags[i] != 'i' && flags[i] != 't' && flags[i] != 'k' && flags[i] != 'o' && flags[i] != 'l') {
					msg = "472 " + curr_user + " ";
					msg += flags[i];
					msg += " is an unknown mode char to me\r\n";
					send(it->fd, msg.c_str(), msg.size(), 0);
					i++;
					continue;
				}
				if (flags[i] == 'i')
					getChannel(channel_name)->setInviteOnly(positive);
				if (flags[i] == 't')
					getChannel(channel_name)->setTopicRestricted(positive);
				if (flags[i] == 'o') {
					if (args.size() > curr_arg) {
						tmp_arg = args[curr_arg++];
						if (tmp_arg[tmp_arg.size() - 1] == '\n')
							tmp_arg = tmp_arg.substr(0, tmp_arg.size() - 1);
						if (!getChannel(channel_name)->isUserInChannel(tmp_arg)) {
							tmp_arg = "441 " + curr_user + " " + tmp_arg + " " + channel_name + " They aren't on that channel\r\n";
							send(it->fd, tmp_arg.c_str(), tmp_arg.size(), 0);
						}
						else {
							if (positive)
								getChannel(channel_name)->addOp(tmp_arg);
							else
								getChannel(channel_name)->removeOp(tmp_arg);
						}

					}
				}
				i++;
			}
			msg = ":" + curr_user + "!~" + curr_user + "@localhost " + msg;
			sendAll(msg); // INSTEAD SEND A SINGLE MESSAGE FOR EACH FLAG IF THEY CHANGE SMTH
		}
	}
	else if (msg == "exit\n" || msg == "shutdown\n")
		stop();
	// flag l = INT LIMIT
	// MODE : fournir les flags avec +/- en un seul arg (-ok ou +ikt par exemple)
	// ARGS DE MODE : /mode <channel> <flags> <args des flags>
}