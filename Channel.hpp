#pragma once

#include "main.hpp"

class User;

class Channel {
	private:
		std::string _name;
		std::vector<User> _users;
		std::vector<User> _op;
	
	public:
		Channel();
		Channel( Channel const &src );
		Channel( std::string name, User &owner );
		~Channel();

		std::string getName();
		void addUser( User &toAdd );
		void removeUser( User &toRemove );
		void addOp( User &toAdd );
		void removeOp( User &toRemove );
		int getUserAmt();
		std::string getUserList();

		Channel &operator=( Channel const &src );
};