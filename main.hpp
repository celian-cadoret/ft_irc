#pragma once

#include <iostream>
#include <cstring>       // Pour memset()
#include <sys/types.h>   // Pour les types de sockets
#include <sys/socket.h>  // Pour les sockets
#include <netinet/in.h>  // Pour les structures d'adresse (sockaddr_in)
#include <unistd.h>      // Pour close()
#include <cstdlib>
#include "Server.hpp"
#include "User.hpp"