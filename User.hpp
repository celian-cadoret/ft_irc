#pragma once

#include "main.hpp"

class User {
	private:
		std::string _username;
		std::string _nickname;
		int _user_fd;
	
	public:
		User();
		User( User const &src );
		User( int user_fd, std::string username, std::string nickname);
		~User();
		int	getUserFd();
		std::string	getUsername();
		std::string	getNickname();

		User &operator=( User const &src );
};