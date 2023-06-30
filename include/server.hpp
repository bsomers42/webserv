#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "socket.hpp"

class Socket;

#define SA struct sockaddr
#define MAXLINE 4096
#define SERVER_PORT 80

class Server
{
	private:
		std::string	contentPath;
	public:
		bool		running;
		bool		start();

};

#endif
