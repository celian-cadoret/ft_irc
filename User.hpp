#pragma once

#include "main.hpp"

class Channel;

class User {
	private:
		int _user_fd;
		std::string _username;
		std::string _nickname;
		int _connect_state;
	
	public:
		User();
		User( User const &src );
		User( int user_fd );
		~User();

		int	getSocket();
		std::string	getUsername();
		std::string	getNickname();
		std::string getPassword();
		int getConnectState();
		void incrementConnectState( int n = 1 );
		void setNickname( std::string nickname );
		void setUsername( std::string username );
		void setSocket( int socket );
		void joinChannel( std::vector<Channel> &channels, std::string name );
		void quitChannel( std::vector<Channel> &channels, std::string name );

		User &operator=( User const &src );
};