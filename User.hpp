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
		~User();

		User &operator=( User const &src );
};