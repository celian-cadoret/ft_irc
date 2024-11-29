#pragma once

#include <iostream>
#include <cstring>       // Pour memset()
#include <sys/types.h>   // Pour les types de sockets
#include <sys/socket.h>  // Pour les sockets
#include <netinet/in.h>  // Pour les structures d'adresse (sockaddr_in)
#include <unistd.h>      // Pour close()
#include <cstdlib>
#include <vector>
#include <map>
#include <poll.h>
#include <algorithm>
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

std::string toLowerStr( std::string s );
std::vector<std::string> splitStr( std::string s, char c );
bool isNickInVector( std::vector<std::string> v, std::string nick );