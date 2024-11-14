#pragma once

#include "main.hpp"

class User {
	private:
		int _user_fd;
		std::string _username;
		std::string _nickname;
		std::string _password;
		bool _op;
		bool _username_set;
		bool _nickname_set;
		bool _password_set;
	
	public:
		User();
		User( User const &src );
		User( int user_fd );
		~User();
		int	getSocket();
		std::string	getUsername();
		std::string	getNickname();
		std::string getPassword();
		void setUsername( std::string username );
		void setNickname( std::string nickname );
		void setPassword( std::string password );

		User &operator=( User const &src );
};