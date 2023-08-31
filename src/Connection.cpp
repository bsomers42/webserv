#include "Webserver.hpp"
#include "Connection.hpp"
#include "Socket.hpp"

Connection::Connection()
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handler = nullptr;
	this->_address = "";
	this->_port = 0;
	// std::cout << "Default constructor called on Connection" << std::endl;
}

Connection::~Connection()
{
	// std::cout << "Destructor called on Connection" << std::endl;
}

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd)
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handler = nullptr;
	this->_address = sckt.getAddress();
	this->_port = sckt.getPort();
	// std::cout << "Parametric constructor called on Connection" << std::endl;
}

Connection::Connection(const Connection &src)
{
	*this = src;
	// std::cout << "Copy constructor called on Connection" << std::endl;
}

Connection& Connection::operator=(const Connection &src)
{
	this->_newReq = src._newReq;
	this->_newResp = src._newResp;
	this->_handler = src._handler;
	this->_listenfd = src._listenfd;
	this->_address = src._address;
	this->_port = src._port;
	// std::cout << "Copy assignment operator called on Connection" << std::endl;
	return (*this);
}

void	Connection::handleRequest(int connfd, std::vector<Server> servers)
{
	try
	{
		if (this->_newReq->getState() == OVERWRITE)
		{
			delete this->_newReq;
			this->_newReq = new Request(connfd, this->_address);
		}
		std::cout << "After Request constructor" << std::endl;
		this->_newReq->processReq();
		this->_newReq->printRequest();
		std::cout << " Address/host in Connection is: [" << this->_address << "]" << std::endl;
		this->_handler = new Server(this->_newReq->identifyServer(servers)); //BS: hier kun je het IP e.d. meenemen uit de Connection class
		std::cout << "_Handler info: host: [" << this->_handler->getPort(0) << "], port: [" << this->_handler->getHost(0) << "]" << std::endl;
		std::cout << "Responsible server is " << \
		this->_handler->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}
static bool	allowedInLocation(std::string method, std::vector<Location>::const_iterator location)
{
	for (size_t i = 0; i < location->getAllowed().size(); i++)
	{
		if (location->getAllowed().at(i) == method)
			return (true);
	}
	for (size_t i = 0; i < location->getDenied().size(); i++)
	{
		if (location->getDenied().at(i) == method || location->getDenied().at(i) == "all")
			return (false);
	}
	return (true);
}

void	Connection::handleResponse()
{
	std::cout << "Responsible SERVER size in handleResponse is " << \
	this->_handler->getServerNames().size() << std::endl;

	try
	{
		this->_newResp = new Response(*this->_newReq);
		this->_newResp->prepareTargetURI(*this->_handler);

		if (!allowedInLocation(this->_newReq->getMethod(), this->_newResp->getLocation()))
		{
			std::cout << "BOOOOH not allowed! " << this->_newReq->getMethod() << " in " << this->_newResp->getLocation()->getMatch() << std::endl;
			this->_newResp->setFilePath("data/www/defaultError.html");
			this->_newReq->setMethod("GET");//should be removed 
			//this should be something that overwrites all variables that matter for the response sending
		}
		//insert tests of allowed methods

		if (this->_newReq->getMethod() == "POST")
		{
			this->_newResp->prepareResponsePOST(*this->_handler);
		}
		else if (this->_newReq->getMethod() == "DELETE" || \
			(this->_newReq->getMethod() == "GET" && \
			this->_newReq->getTarget() == "/deleted.html" && \
			this->_newReq->getQueryString() != ""))
		{
			this->_newResp->prepareResponseDELETE(*this->_handler);
		}	
		else if (this->_newReq->getMethod() == "GET")
		{
			;
		}
		else
		{
			std::cout << "I can't handle the "" method, sorry!" << std::endl;
		}

		//RETURN CHECK
		if (this->_newReq->getMethod() == "GET" || this->_newReq->getMethod() == "DELETE")
		{
			// if (this->_newResp->getLocation()->getReturn().first != 0)
			// {
			// 	std::cout << "RETURN!!!!!!!!!!" << std::endl;
			// 	// uint8_t	response[MAXLINE + 1];
			// 	// std::memset(response, 0, MAXLINE);

			// 	// snprintf((char *)response, MAXLINE, "%s 301 %s\r\n", this->_newReq->getProtocolVersion().c_str(), /*this->_newResp->getLocation()->getReturn().first,*/ (this->_newResp->getLocation()->getReturn().second).c_str());
			// 	// std::cout << "Return about to send: " << (char*)response << std::endl;
			// 	// send(this->_newReq->getConnFD(), (char*)response, std::strlen((char *)response), 0);
			// }
			// else
			// {
				if (this->_newReq->getMethod() == "GET")
					this->_newResp->retrieveFile(this->_newResp->getLocation()->getRoot());
				else
					send(this->_newReq->getConnFD(), (char*)this->_newResp->getFullResponse(), std::strlen((char *)this->_newResp->getFullResponse()), 0);
			// }
		}
		this->_newReq->setState(OVERWRITE);
		delete this->_newResp;
		delete this->_handler;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}

void	Connection::setRequest(Request *request)
{
	this->_newReq = request;
}

Request *	Connection::getRequest(void)
{
	return (this->_newReq);
}
