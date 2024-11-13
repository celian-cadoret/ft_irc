#pragma once

#include "main.hpp"

class User;

class Server {
	private:
		int _port;
		std::string _password;
		int _server_fd;
		struct sockaddr_in _address;

		User _user;
	
	public:
		Server();
		Server( Server const &src );
		Server( int port, std::string password );
		~Server();

		Server &operator=( Server const &src );

		int getPort();
		std::string getPassword();
		int &getSocket();
		struct sockaddr_in &getAddress();
		void	addUser(User &user);
		User &getUser();

		void start();
		void	connectUser();
};