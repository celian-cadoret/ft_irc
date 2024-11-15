#pragma once

#include "main.hpp"

class User;

class Server {
	private:
		int _port;
		std::string _password;
		int _server_fd;
		struct sockaddr_in _address;
		std::vector<User> _user;
	
	public:
		Server();
		Server( Server const &src );
		Server( int port, std::string password );
		~Server();

		Server &operator=( Server const &src );

		int getPort();
		std::string getPassword();
		int getSocket();
		struct sockaddr_in &getAddress();
		User &getUser( int i );
		int getUserAmt();
		int getUserFromSocket( int socket );

		void start();
		void connectUser( std::vector<pollfd> &new_pollfds );
		void manageUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it );
		void deleteUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it );
};