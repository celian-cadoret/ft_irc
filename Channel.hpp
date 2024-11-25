#pragma once

#include "main.hpp"

class User;

class Channel {
	private:
		std::string _name;
		std::map<std::string, bool*> _user; // bool[0] = online, bool[1] = op
	
	public:
		Channel();
		Channel( Channel const &src );
		Channel( std::string name, std::string owner );
		~Channel();

		std::string getName();
		void addUser( std::string toAdd );
		void removeUser( std::string toRemove );
		void addOp( std::string toAdd );
		void removeOp( std::string toRemove );
		void setUserOnline( std::string nickname );
		void setUserOffline( std::string nickname );
		int getUserAmt();
		std::string getUserList();
		std::map<std::string, bool*> &getUsers();
		bool *getUserDatas( std::string nickname );
		bool isUserInChannel( std::string nickname );

		Channel &operator=( Channel const &src );
};