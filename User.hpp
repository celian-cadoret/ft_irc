#pragma once

#include "main.hpp"

class User {
	private:
		int _user_fd;
		std::string _username;
		std::string _nickname;
	
	public:
		User();
		User( User const &src );
		User( int user_fd, std::string username, std::string nickname );
		~User();
		int	getSocket();
		std::string	getUsername();
		std::string	getNickname();

		User &operator=( User const &src );
};