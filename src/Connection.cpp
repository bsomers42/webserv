#include "Webserver.hpp"
#include "Connection.hpp"
#include "Socket.hpp"

Connection::Connection()
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handlingServer = nullptr;
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
	this->_handlingServer = nullptr;
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
	this->_handlingServer = src._handlingServer;
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
		// std::cout << "After Request constructor" << std::endl;
		this->_newReq->processReq();
		this->_newReq->printRequest();
		this->_handlingServer = new Server(this->_newReq->identifyServer(servers));
		std::cout << "_Handler info: host: [" << this->_handlingServer->getPort(0) << "], port: [" << this->_handlingServer->getHost(0) << "]" << std::endl;
		std::cout << "Responsible server is " << \
		this->_handlingServer->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! PROBABLY STH NEEDS TO BE ADDED TO CONFIG FILE - " << e.what() << '\n';
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
	if (this->_newReq->getMethod() == "")
		return;

	// if unknown response type, return BAD REQUEST

	try
	{
		this->_newResp = new Response(*this->_newReq);
		this->_newResp->prepareTargetURI(*this->_handlingServer);

		if (this->_newReq->getMethod() == "GET" && this->_newReq->getTarget() == "/deleted.html" && this->_newReq->getQueryString() != "")
			this->_newReq->setMethod("DELETE");
		if (!allowedInLocation(this->_newReq->getMethod(), this->_newResp->getLocation()))
		{
			std::cout << "Method not allowed! " << this->_newReq->getMethod() << " in " << this->_newResp->getLocation()->getMatch() << std::endl;
			if (this->_newResp->getRequest().getHeaders()["User-Agent"].find("curl") == 0)
			{
				this->_newResp->setFilePath("");
			}
			else
			{
				this->_newResp->setFilePath("data/www/defaultError.html");
			}
			this->_newResp->setStatusCode(NOT_ALLOWED);
			//this should be something that overwrites all variables that matter for the response sending
		}
		else
		{
			if (this->_newReq->getMethod() == "POST")
			{
				this->_newResp->prepareResponsePOST();
			}
			else if (this->_newReq->getMethod() == "DELETE")
			{
				this->_newResp->prepareResponseDELETE();
			}	
			else if (this->_newReq->getMethod() != "GET")
			{
				std::cout << "I can't handle the \"" << this->_newReq->getMethod() << "\" method, sorry!" << std::endl;
				return;
			}
		}

		if (this->_newResp->getLocation()->getReturn().first != 0)
		{
			std::cout << "RETURN!!!!!!!!!! " << this->_newResp->getLocation()->getReturn().first << " " << this->_newResp->getLocation()->getReturn().second << std::endl;
			this->_newResp->setStatusCode(this->_newResp->getLocation()->getReturn().first);
			this->_newResp->setMessage(this->_newResp->getLocation()->getReturn().second);
			this->_newResp->setFilePath("");
			std::cout << "done return overwriting. JMA: I think it should be sent below differently, let's discuss this" << std::endl;
		}
		this->_newResp->sendResponse();
		this->_newReq->setState(OVERWRITE);
		delete this->_newResp;
		delete this->_handlingServer;
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
