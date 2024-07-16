/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:27 by jbarbay           #+#    #+#             */
/*   Updated: 2024/07/16 18:23:16 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response() {}

Response::Response(Request &request)
{
	if (request.getMethod() == "GET")
		this->respond_get_request(request);

	else if (request.getMethod() == "POST")
		this->respond_post_request(request);

	else if (request.getMethod() == "DELETE")
		this->respond_delete_request(request);
		
	else
		this->respond_wrong_request(request);

	_headers["Cache-Control"] = "no-cache, private";
	this->_http_version = request.getHttpVersion();

	if (this->_http_version == "HTTP/1.1" && request.getHeaders()["Connection"] == "keep-alive")
		_headers["Connection"] = "keep-alive";
	else
		_headers["Connection"] = "close";

	setContentType(request);
	getDate();
	setFullResponse();

	//  Content-Length if there is a body
	// /!\ Body can be empty string, would be valid request
}

Response::Response( const Response & src ) :
_status_code(src._status_code),
_status_text(src._status_text),
_http_version(src._http_version),
_body(src._body),
_full_response(src._full_response),
_headers(src._headers)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Response::respond_get_request(const Request &request)
{
	char						c;
	std::ifstream				page(request.getPath().c_str());

	if (page.good())
	{
		while (page.get(c))
			_body += c;
		this->_status_code = 200;
		this->_status_text = "OK";
	}
	else
	{
		this->_status_code = 404;
		this->_status_text = "Not found";
		_body = get_error_page(_status_code);
	}
	_headers["Content-Length"] = intToString(this->_body.size());
	page.close();
}

void	Response::respond_post_request(const Request &request)
{
	std::string	name = "Juliette";
	std::string	email = "hello@gmail.com";
	if (request.getHeaders()["Content-Type"] == "application/x-www-form-urlencoded")
	{
		std::cout << "Wrong type\n";
	}
	else //(request.getHeaders()["Content-Type"] == "multipart/form-data")
	{
		if (request.getPath() == "./wwwroot/simple-form.html")
		{
			_body = "<p>Saved " + name + "</p>";
		}
		else if (request.getPath() == "./wwwroot/subscribe.html")
		{
			std::cout << "hereeeee" << std::endl;
			addToData(email);
		}
	}
}

void	Response::respond_delete_request(const Request &request)
{
	(void)request;
}

void	Response::respond_wrong_request(const Request &request)
{
	(void)request;
	this->_status_code = 405;
	this->_status_text = "Method Not Allowed";
	_headers["Allow"] = "GET, POST, DELETE";
}

void		Response::getDate()
{
    time_t time;
    std::time(&time);

    struct tm *gmt;
    gmt = std::gmtime(&time);
    char formatted_date[30];
    std::strftime(formatted_date, sizeof(formatted_date), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    // std::cout << std::string(formatted_date) << std::endl;
	_headers["Date"] = formatted_date;
}

void	Response::addToData(std::string data)
{
	std::ofstream file;
	file.open("./database/newsletter.txt", std::ios::app);
	if (!file.is_open())
		std::cout << strerror(errno);
	file << data << "\n";
	file.close();
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Response &				Response::operator=( Response const & rhs )
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
	}
	return (*this);
}

std::string		Response::get_error_page(int num)
{
	std::stringstream 			ss;
	ss << num;
	std::string					string_code = ss.str();
	
	std::string					error_path = "./errors/" + intToString(num) + ".html";
	std::ifstream				error_page(error_path.c_str());
	std::string					str;
	char						c;

	if (error_page.good())
	{
		while (error_page.get(c))
			str += c;
	}
	else
	{
		std::cout << "Error loading Error page " << num << std::endl;
		str = "<h1>Error" +  intToString(num) + "</h1><p>" + _status_text + "</p>";
	}
	error_page.close();
	return (str);
}

std::string	Response::intToString(int num)
{
	std::stringstream	stream;
	stream << num;
	return (stream.str());
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Response::getStatusCode() const
{
	return (this->_status_code);
}

std::string	Response::getStatusText() const
{
	return (this->_status_text);
}

std::string	Response::getHttpVersion() const
{
	return (this->_http_version);
}

std::map<std::string, std::string>	Response::getHeaders() const
{
	return (this->_headers);
}

std::string	Response::getBody() const
{
	return (this->_body);
}

std::string	Response::getFullResponse() const
{
	return (this->_full_response);
}

void	Response::setFullResponse()
{
	std::stringstream								stream;
	std::map<std::string, std::string>::iterator	it;

	stream << this->_http_version << " " << this->_status_code << " " << this->_status_text << "\r\n";

	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		stream << it->first << ": " << it->second << "\r\n";
	}
	stream << "\r\n";
	stream << this->_body;

	this->_full_response = stream.str();
}

void	Response::setContentType(Request &request)
{
	std::string		path = request.getPath();
	std::size_t		pos = path.find_last_of(".");
	std::string		ext;

	if (pos == std::string::npos)
	{
		_headers["Content-Type"] = "text/plain";
		return ;
	}
	ext = path.substr(pos + 1, path.size() - pos + 1);
	if (ext == "html" || ext == "htm")
		_headers["Content-Type"] = "text/html";
	else if (ext == "css")
		_headers["Content-Type"] = "text/css";
	else if (ext == "js")
		_headers["Content-Type"] = "text/javascript";
	else if (ext == "jpg" || ext == "jpeg")
		_headers["Content-Type"] = "image/jpeg";
	else if (ext == "png")
		_headers["Content-Type"] = "image/png";
	else if (ext == "gif")
		_headers["Content-Type"] = "image/gif";
	else if (ext == "pdf")
		_headers["Content-Type"] = "application/pdf";
	else
		_headers["Content-Type"] = "text/plain";
	// std::cout << "Extension: " << ext << std::endl;
	// std::cout << "Path: " << path << std::endl;
}

/* ************************************************************************** */