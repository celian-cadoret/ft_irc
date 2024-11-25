#pragma once

#include "main.hpp"

class User;
class Channel;

class Server {
	private:
		std::string _name;
		int _port;
		std::string _password;
		int _server_fd;
		struct sockaddr_in _address;
		std::vector<User> _user;
		std::vector<Channel> _channels;
	
	public:
		Server();
		Server( Server const &src );
		Server( std::string name, int port, std::string password );
		~Server();

		Server &operator=( Server const &src );

		int getPort();
		std::string getPassword();
		int getSocket();
		struct sockaddr_in &getAddress();
		User &getUser( int i );
		int getUserAmt();
		int getUserFromSocket( int socket );
		Channel &getChannel( std::string name );

		void start();
		void connectUser( std::vector<pollfd> &new_pollfds );
		void manageUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it );
		std::vector<pollfd>::iterator deleteUser( std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it );
		void joinChannelClient( std::vector<pollfd>::iterator &it, std::string name );

		void sendAll( std::string buff, int ignore = 0 );
};